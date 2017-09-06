#include "stdafx.h"
#include "DriverMessenger.h"




DriverMessenger::DriverMessenger(boost::asio::io_service& io):
	_running{true},
	_process([](void const*, std::size_t) {}),
	m_sentinelTimer(io),
	m_sentinelInterval(1000)

{

	OwnedReadableSharedQueue::remove("ns-haptics-data");
	WritableSharedObject<NullSpace::SharedMemory::TrackingUpdate>::remove("ns-tracking-data");
	OwnedWritableSharedQueue::remove("ns-logging-data");
	OwnedReadableSharedQueue::remove("ns-command-data");
	WritableSharedObject<std::time_t>::remove("ns-sentinel");
	OwnedWritableSharedVector<NullSpace::SharedMemory::RegionPair>::remove("ns-bodyview-mem");
	OwnedWritableSharedVector<NullSpace::SharedMemory::DeviceInfo>::remove("ns-device-mem");

	constexpr int systemInfoSize = sizeof(NullSpace::SharedMemory::DeviceInfo);

	m_systems = std::make_unique<OwnedWritableSharedVector<NullSpace::SharedMemory::DeviceInfo>>("ns-device-mem", "ns-device-data", systemInfoSize*32);
	m_tracking = std::make_unique<OwnedWritableSharedMap<uint32_t, NullSpace::SharedMemory::Quaternion>>(/* initial element capacity*/16, "ns-tracking-2");
	m_bodyView = std::make_unique<OwnedWritableSharedVector<NullSpace::SharedMemory::RegionPair>>("ns-bodyview-mem", "ns-bodyview-vec", 2048);
	m_hapticsData = std::make_unique<OwnedReadableSharedQueue>("ns-haptics-data", /*max elements*/1024, /* max element byte size*/512);
	m_trackingData = std::make_unique<WritableSharedObject<NullSpace::SharedMemory::TrackingUpdate>>("ns-tracking-data");
	m_loggingStream = std::make_unique<OwnedWritableSharedQueue>("ns-logging-data", /* max elements */ 512, /*max element byte size*/ 512);
	m_commandStream = std::make_unique<OwnedReadableSharedQueue>("ns-command-data",/* max elements */  512, /*max element byte size*/ 512);
	static_assert(sizeof(std::time_t) == 8, "Time is wrong size");
	
	m_sentinel = std::make_unique<WritableSharedObject<std::time_t>>("ns-sentinel");



	
	startSentinel();
}


void DriverMessenger::startSentinel() {
	m_sentinelTimer.expires_from_now(m_sentinelInterval);
	m_sentinelTimer.async_wait([&](auto error) {sentinelHandler(error); });
}

void DriverMessenger::UpdateDeviceStatus(uint32_t id, DeviceStatus status)
{
	if (auto index = m_systems->Find([id](const DeviceInfo& s) {return s.Id == id; })) {
		auto currentCopy = m_systems->Get(*index);
		currentCopy.Status = status;
		m_systems->Update(*index, currentCopy);
	}
	else {
		BOOST_LOG_TRIVIAL(warning) << "[Messenger] Unable to update device " << id << " status, because it doesn't exist";
	}

}

void DriverMessenger::sentinelHandler(const boost::system::error_code& ec) {
	if (!ec) {
		m_sentinel->Write(std::time(nullptr));
		startSentinel();
	}
}

DriverMessenger::~DriverMessenger()
{




}

//Precondition: The keys were initialized already using Insert on m_tracking
void DriverMessenger::WriteTracking(uint32_t region, NullSpace::SharedMemory::Quaternion quat)
{
	if (m_tracking->Contains(region)) {
		m_tracking->Update(region, quat);
	}
	else {
		m_tracking->Insert(region, quat);
	}
}



void DriverMessenger::WriteDevice(const DeviceInfo&  system)
{
	auto sameId = [&system](const DeviceInfo& s) { return s.Id == system.Id; };
	if (auto index = m_systems->Find(sameId))
	{
		m_systems->Update(*index, system);
	}
	else {
		m_systems->Push(system);
	}
}

void DriverMessenger::RemoveDevice(uint32_t id)
{
	m_systems->Remove([id](const DeviceInfo& s) {return s.Id == id; });
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

void DriverMessenger::WriteLog(std::string s)
{
	//assumes null termination
	if (!m_loggingStream->Push(s.data(), s.length() + 1)) {
		std::cout << "[DriverMessenger] Failed to write to gameplay log\n";
		
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


