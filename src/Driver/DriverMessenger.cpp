#include "stdafx.h"
#include "DriverMessenger.h"
#include "Encoder.h"



DriverMessenger::DriverMessenger(boost::asio::io_service& io):
	_running{true},
	_process([](void const*, std::size_t) {}),
	m_sentinelTimer(io),
	m_sentinelInterval(1000)

{

	OwnedReadableSharedQueue::remove("ns-haptics-data");
	WritableSharedObject<NullSpace::SharedMemory::TrackingUpdate>::remove("ns-tracking-data");
	WritableSharedObject<SuitsConnectionInfo>::remove("ns-suit-data");
	OwnedWritableSharedQueue::remove("ns-logging-data");
	OwnedReadableSharedQueue::remove("ns-command-data");
	WritableSharedObject<std::time_t>::remove("ns-sentinel");
	


	m_tracking = std::make_unique<OwnedWritableSharedTracking>();
	m_hapticsData = std::make_unique<OwnedReadableSharedQueue>("ns-haptics-data", /*max elements*/1024, /* max element byte size*/512);
	m_trackingData = std::make_unique<WritableSharedObject<NullSpace::SharedMemory::TrackingUpdate>>("ns-tracking-data");
	m_suitConnectionInfo = std::make_unique<WritableSharedObject<SuitsConnectionInfo>>("ns-suit-data");
	m_loggingStream = std::make_unique<OwnedWritableSharedQueue>("ns-logging-data", /* max elements */ 512, /*max element byte size*/ 512);
	m_commandStream = std::make_unique<OwnedReadableSharedQueue>("ns-command-data",/* max elements */  512, /*max element byte size*/ 512);
	static_assert(sizeof(std::time_t) == 8, "Time is wrong size");
	
	m_sentinel = std::make_unique<WritableSharedObject<std::time_t>>("ns-sentinel");

	TrackingUpdate nullTracking = {};
	SuitsConnectionInfo nullSuits = {};

	NullSpace::SharedMemory::Quaternion nullQuat = {};
	
	m_tracking->Insert("chest", nullQuat);
	m_tracking->Insert("left_upper_arm", nullQuat);
	m_tracking->Insert("right_upper_arm", nullQuat);

	m_suitConnectionInfo->Write(nullSuits);
	
	startSentinel();
}


void DriverMessenger::startSentinel() {
	m_sentinelTimer.expires_from_now(m_sentinelInterval);
	m_sentinelTimer.async_wait([&](auto error) {sentinelHandler(error); });
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
void DriverMessenger::WriteTracking(const std::string& region, NullSpace::SharedMemory::Quaternion quat)
{
	m_tracking->Update(region.c_str(), quat);
}

void DriverMessenger::WriteSuits(SuitsConnectionInfo s)
{
	m_suitConnectionInfo->Write(s);
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


