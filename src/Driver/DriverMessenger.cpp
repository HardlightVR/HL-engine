#include "stdafx.h"
#include <boost/optional.hpp>
#include "DriverMessenger.h"
#include "runtime_include/NSDriverApi.h"
#include "logger.h"


DriverMessenger::DriverMessenger(boost::asio::io_service& io):
	_running{true},
	_process([](void const*, std::size_t) {}),
	m_sentinelTimer(io),
	m_sentinelInterval(1000)

{
	static_assert(sizeof(char) == 1, "set char size to 1");

	OwnedReadableSharedQueue::remove("ns-haptics-data");
	WritableSharedObject<NullSpace::SharedMemory::TrackingUpdate>::remove("ns-tracking-data");
	OwnedWritableSharedQueue::remove("ns-logging-data");
	OwnedReadableSharedQueue::remove("ns-command-data");
	WritableSharedObject<NullSpace::SharedMemory::SentinelObject>::remove("ns-sentinel");
	OwnedWritableSharedVector<NullSpace::SharedMemory::RegionPair>::remove("ns-bodyview-mem");
	OwnedWritableSharedVector<NullSpace::SharedMemory::NodeInfo>::remove("ns-node-mem");
	OwnedWritableSharedVector<NullSpace::SharedMemory::DeviceInfo>::remove("ns-device-mem");
	OwnedWritableSharedMap<uint32_t, NullSpace::SharedMemory::Quaternion>::remove("ns-tracking-2");
	constexpr int systemInfoSize = sizeof(NullSpace::SharedMemory::DeviceInfo);
	constexpr int nodeInfoSize = sizeof(NullSpace::SharedMemory::NodeInfo);

	constexpr int regionPairSize = sizeof(NullSpace::SharedMemory::RegionPair);
	
	constexpr int numberOfSystemsUpperBound = 32;
	constexpr int averageNodesPerSystem = 32;
	m_nodes = std::make_unique<OwnedWritableSharedVector<NullSpace::SharedMemory::NodeInfo>>("ns-node-mem", "ns-node-data", nodeInfoSize * averageNodesPerSystem * numberOfSystemsUpperBound );

	m_devices = std::make_unique<OwnedWritableSharedVector<NullSpace::SharedMemory::DeviceInfo>>("ns-device-mem", "ns-device-data", systemInfoSize*numberOfSystemsUpperBound);
	m_tracking = std::make_unique<OwnedWritableSharedMap<uint32_t, NullSpace::SharedMemory::Quaternion>>("ns-tracking-2");
	m_bodyView = std::make_unique<OwnedWritableSharedVector<NullSpace::SharedMemory::RegionPair>>("ns-bodyview-mem", "ns-bodyview-vec", regionPairSize*numberOfSystemsUpperBound*averageNodesPerSystem);
	m_hapticsData = std::make_unique<OwnedReadableSharedQueue>("ns-haptics-data", /*max elements*/1024, /* max element byte size*/512);
	m_loggingStream = std::make_unique<OwnedWritableSharedQueue>("ns-logging-data", /* max elements */ 512, /*max element byte size*/ 512);
	m_commandStream = std::make_unique<OwnedReadableSharedQueue>("ns-command-data",/* max elements */  512, /*max element byte size*/ 512);
	static_assert(sizeof(std::time_t) == 8, "Time is wrong size");
	
	m_sentinel = std::make_unique<WritableSharedObject<NullSpace::SharedMemory::SentinelObject>> ("ns-sentinel");



	
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



//Precondition: The keys were initialized already using Insert on m_tracking
void DriverMessenger::WriteTracking(uint32_t region, const NullSpace::SharedMemory::Quaternion& quat)
{
	m_tracking->Update(region, quat);	
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


boost::optional<std::vector<NullSpaceIPC::EffectCommand>> DriverMessenger::ReadHaptics()
{
	return readFromStream<NullSpaceIPC::EffectCommand, OwnedReadableSharedQueue>(*m_hapticsData.get(), 100);
}

boost::optional<std::vector<NullSpaceIPC::HighLevelEvent>> DriverMessenger::ReadEvents()
{
	return readFromStream<NullSpaceIPC::HighLevelEvent, OwnedReadableSharedQueue>(*m_hapticsData.get(), 100);
}



boost::optional<std::vector<NullSpaceIPC::DriverCommand>> DriverMessenger::ReadCommands()
{

	return readFromStream<NullSpaceIPC::DriverCommand, OwnedReadableSharedQueue>(*m_commandStream.get(), 100);
}




void DriverMessenger::Disconnect()
{
	_running.store(false);
	m_sentinelTimer.cancel();
}


