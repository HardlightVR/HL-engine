#include "stdafx.h"
#include <boost/optional.hpp>
#include "DriverMessenger.h"
#include "runtime_include/NSDriverApi.h"
#include "logger.h"
#include "BoostIPCSharedMemoryDirectory.h"

DriverMessenger::DriverMessenger(boost::asio::io_service& io):
	_running{true},
	_process([](void const*, std::size_t) {}),
	m_sentinelTimer(io),
	m_sentinelInterval(1000)

{
	static_assert(sizeof(char) == 1, "set char size to 1");

	OwnedReadableSharedQueue::remove("ns-haptics-data");
	OwnedWritableSharedQueue::remove("ns-logging-data");
	WritableSharedObject<NullSpace::SharedMemory::SentinelObject>::remove("ns-sentinel");
	OwnedWritableSharedVector<NullSpace::SharedMemory::RegionPair>::remove("ns-bodyview-mem");
	OwnedWritableSharedVector<NullSpace::SharedMemory::NodeInfo>::remove("ns-node-mem");
	OwnedWritableSharedVector<NullSpace::SharedMemory::DeviceInfo>::remove("ns-device-mem");
	OwnedWritableSharedVector<NullSpace::SharedMemory::TrackingData>::remove("ns-tracking-mem");
	constexpr int systemInfoSize = sizeof(NullSpace::SharedMemory::DeviceInfo);
	constexpr int nodeInfoSize = sizeof(NullSpace::SharedMemory::NodeInfo);

	constexpr int regionPairSize = sizeof(NullSpace::SharedMemory::RegionPair);
	
	constexpr int numberOfSystemsUpperBound = 32;
	constexpr int averageNodesPerSystem = 32;
	try {
		m_nodes = std::make_unique<OwnedWritableSharedVector<NullSpace::SharedMemory::NodeInfo>>("ns-node-mem", "ns-node-data", nodeInfoSize * averageNodesPerSystem * numberOfSystemsUpperBound);

		m_devices = std::make_unique<OwnedWritableSharedVector<NullSpace::SharedMemory::DeviceInfo>>("ns-device-mem", "ns-device-data", systemInfoSize*numberOfSystemsUpperBound);
		m_tracking = std::make_unique<OwnedWritableSharedVector<NullSpace::SharedMemory::TrackingData>>("ns-tracking-mem", "ns-tracking-data", sizeof(NullSpace::SharedMemory::TrackingData) * 16 + 2048 /* extra bytes for storing the overhead of a vector*/);
		m_bodyView = std::make_unique<OwnedWritableSharedVector<NullSpace::SharedMemory::RegionPair>>("ns-bodyview-mem", "ns-bodyview-data", regionPairSize*numberOfSystemsUpperBound*averageNodesPerSystem);
		m_hapticsData = std::make_unique<OwnedReadableSharedQueue>("ns-haptics-data", /*max elements*/1024, /* max element byte size*/512);
		m_loggingStream = std::make_unique<OwnedWritableSharedQueue>("ns-logging-data", /* max elements */ 512, /*max element byte size*/ 512);
		static_assert(sizeof(std::time_t) == 8, "Time is wrong size");

		m_sentinel = std::make_unique<WritableSharedObject<NullSpace::SharedMemory::SentinelObject>>("ns-sentinel");

	}
	catch (const boost::interprocess::interprocess_exception& ex) {
		BOOST_LOG_SEV(clogger::get(), nsvr_severity_fatal) << "Unable to initialize shared memory bridge; shutting down!";
		throw;
	}

	
	startSentinel();
}


void DriverMessenger::startSentinel() {
	m_sentinelTimer.expires_from_now(m_sentinelInterval);
	m_sentinelTimer.async_wait([&](auto error) {sentinelHandler(error); });
}

void DriverMessenger::UpdateDeviceStatus(uint32_t id, DeviceStatus status)
{
	if (auto index = m_devices->Find([id](const DeviceInfo& s) {return s.Id == id; })) {
		auto currentCopy = m_devices->Get(*index);
		currentCopy.Status = status;
		m_devices->Update(*index, currentCopy);
	}
	else {
		BOOST_LOG_TRIVIAL(warning) << "[Messenger] Unable to update device " << id << " status, because it doesn't exist";
	}

}

void DriverMessenger::sentinelHandler(const boost::system::error_code& ec) {
	if (!ec) {
		m_sentinel->Write(
			NullSpace::SharedMemory::SentinelObject{ 
				NullSpace::SharedMemory::ServiceInfo {HVR_PLATFORM_API_VERSION_MAJOR, HVR_PLATFORM_API_VERSION_MINOR}, 
				std::time(nullptr) 
			}
		);
		startSentinel();
	}
}

struct by_region {
	by_region(uint32_t region) : region(region) {}
	bool operator()(const NullSpace::SharedMemory::TrackingData& quat) {
		return quat.region == region;
	}
	uint32_t region;
};

//Precondition: The keys were initialized already using Insert on m_tracking
void DriverMessenger::WriteQuaternion(uint32_t region, const NullSpace::SharedMemory::Quaternion& quat)
{

	if (auto index = m_tracking->Find(by_region(region))) {
		m_tracking->Mutate(*index, [=](NullSpace::SharedMemory::TrackingData& existing) {
			existing.quat = quat;
		});
	}
	else {
		m_tracking->Push(NullSpace::SharedMemory::TrackingData::withQuat(region, quat));
	}
}

void DriverMessenger::WriteCompass(uint32_t region, const NullSpace::SharedMemory::Vector3 & vec)
{
	if (auto index = m_tracking->Find(by_region(region))) {
		m_tracking->Mutate(*index, [=](NullSpace::SharedMemory::TrackingData& existing) {
			existing.compass = vec;
		});
	}
	else {
		m_tracking->Push(NullSpace::SharedMemory::TrackingData::withCompass(region, vec));
	}
}

void DriverMessenger::WriteGravity(uint32_t region, const NullSpace::SharedMemory::Vector3 & vec)
{
	if (auto index = m_tracking->Find(by_region(region))) {
		m_tracking->Mutate(*index, [=](NullSpace::SharedMemory::TrackingData& existing) {
			existing.gravity = vec;
		});
	}
	else {
		m_tracking->Push(NullSpace::SharedMemory::TrackingData::withGrav(region, vec));
	}
}



void DriverMessenger::WriteDevice(const DeviceInfo&  system)
{
	auto sameId = [&system](const DeviceInfo& s) { return s.Id == system.Id; };
	if (auto index = m_devices->Find(sameId))
	{
		m_devices->Update(*index, system);
	}
	else {
		m_devices->Push(system);
	}
}

void DriverMessenger::WriteNode(const NodeInfo & node)
{
	auto sameId = [&node](const NodeInfo& s) { return s.Id == node.Id; };
	if (auto index = m_nodes->Find(sameId))
	{
		m_nodes->Update(*index, node);
	}
	else {
		m_nodes->Push(node);
	}

}

void DriverMessenger::RemoveNode(uint64_t id)
{
	m_nodes->Remove([id](const NodeInfo& s) {return s.Id == id; });

}

void DriverMessenger::RemoveDevice(uint32_t id)
{
	m_devices->Remove([id](const DeviceInfo& s) {return s.Id == id; });
}

void DriverMessenger::WriteBodyView(NullSpace::SharedMemory::RegionPair data)
{
	if (auto index = m_bodyView->Find(data)) {
		m_bodyView->Update(*index, std::move(data));
	}
	else {
		m_bodyView->Push(std::move(data));
	}
	
}



boost::optional<std::vector<NullSpaceIPC::HighLevelEvent>> DriverMessenger::ReadEvents()
{
	return readFromStream<NullSpaceIPC::HighLevelEvent, OwnedReadableSharedQueue>(*m_hapticsData.get(), 100);
}







void DriverMessenger::Disconnect()
{
	_running.store(false);
	m_sentinelTimer.cancel();
}


