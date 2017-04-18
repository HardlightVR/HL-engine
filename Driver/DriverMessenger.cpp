#include "stdafx.h"
#include "DriverMessenger.h"
#include "Encoder.h"
#include <boost\log\trivial.hpp>
DriverMessenger::DriverMessenger(boost::asio::io_service& io):
	_running{true},
	_process([](void const*, std::size_t) {}),
	m_sentinalTimer(io),
	m_sentinalInterval(1000)

{

	OwnedReadableSharedQueue::remove("ns-haptics-data");
	WritableSharedObject<NullSpace::SharedMemory::TrackingUpdate>::remove("ns-tracking-data");
	WritableSharedObject<SuitsConnectionInfo>::remove("ns-suit-data");
	OwnedWritableSharedQueue::remove("ns-logging-data");
	OwnedWritableSharedQueue::remove("ns-command-data");
	WritableSharedObject<std::time_t>::remove("ns-sentinel");

	m_hapticsData = std::make_unique<OwnedReadableSharedQueue>("ns-haptics-data", 1024, 256);
	m_trackingData = std::make_unique<WritableSharedObject<NullSpace::SharedMemory::TrackingUpdate>>("ns-tracking-data");
	m_suitConnectionInfo = std::make_unique<WritableSharedObject<SuitsConnectionInfo>>("ns-suit-data");
	m_loggingStream = std::make_unique<OwnedWritableSharedQueue>("ns-logging-data", /* max elements */ 512, /*max element byte size*/ 512);
	m_commandStream = std::make_unique<OwnedReadableSharedQueue>("ns-command-data",/* max elements */  512, /*max element byte size*/ 256);

	m_sentinal = std::make_unique<WritableSharedObject<std::time_t>>("ns-sentinel");

	TrackingUpdate nullTracking = {};
	SuitsConnectionInfo nullSuits = {};
	m_trackingData->Write(nullTracking);
	m_suitConnectionInfo->Write(nullSuits);
	
	startSentinal();
}


void DriverMessenger::startSentinal() {
	m_sentinalTimer.expires_from_now(m_sentinalInterval);
	m_sentinalTimer.async_wait(boost::bind(&DriverMessenger::sentinalHandler, this, boost::asio::placeholders::error));
}
void DriverMessenger::sentinalHandler(const boost::system::error_code& ec) {
	if (!ec) {
		m_sentinal->Write(std::time(nullptr));
		startSentinal();
	}
}

DriverMessenger::~DriverMessenger()
{




}

void DriverMessenger::WriteTracking(TrackingUpdate t)
{
	m_trackingData->Write(t);
}

void DriverMessenger::WriteSuits(SuitsConnectionInfo s)
{
	m_suitConnectionInfo->Write(s);
}

void DriverMessenger::WriteLog(std::string s)
{
	//assumes null termination
	if (!m_loggingStream->Push(s.data(), s.length() + 1)) {
		std::cout << "Failed to push to log stream";
	}
}

boost::optional<std::vector<NullSpaceIPC::EffectCommand>> DriverMessenger::ReadHaptics()
{
	return readFromStream<NullSpaceIPC::EffectCommand, OwnedReadableSharedQueue>(*m_hapticsData.get(), 100);
}

boost::optional<std::vector<NullSpaceIPC::DriverCommand>> DriverMessenger::ReadCommands()
{

	return readFromStream<NullSpaceIPC::DriverCommand, OwnedReadableSharedQueue>(*m_commandStream.get(), 100);
}




void DriverMessenger::Disconnect()
{
	_running.store(false);
	m_sentinalTimer.cancel();
}


