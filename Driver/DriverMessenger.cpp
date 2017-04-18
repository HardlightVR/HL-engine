#include "stdafx.h"
#include "DriverMessenger.h"
#include "Encoder.h"
#include <boost\log\trivial.hpp>
DriverMessenger::DriverMessenger(boost::asio::io_service& io):
_running{true},
	_process([](void const*, std::size_t) {}),
	m_hapticsData("ns-haptics-data", 1024, 256),
	m_trackingData("ns-tracking-data"),
	m_suitConnectionInfo("ns-suit-data"),
	m_loggingStream("ns-logging-data", /* max elements */ 512, /*max element byte size*/ 512),
	m_commandStream("ns-command-data",/* max elements */  512, /*max element byte size*/ 256),

	m_sentinal("ns-sentinel"),

	m_sentinalTimer(io),
	m_sentinalInterval(1000)

{
	TrackingUpdate nullTracking = {};
	SuitsConnectionInfo nullSuits = {};
	m_trackingData.Write(nullTracking);
	m_suitConnectionInfo.Write(nullSuits);
	
	startSentinal();
}


void DriverMessenger::startSentinal() {
	m_sentinalTimer.expires_from_now(m_sentinalInterval);
	m_sentinalTimer.async_wait(boost::bind(&DriverMessenger::sentinalHandler, this, boost::asio::placeholders::error));
}
void DriverMessenger::sentinalHandler(const boost::system::error_code& ec) {
	if (!ec) {
		m_sentinal.Write(std::time(nullptr));
		startSentinal();
	}
}

DriverMessenger::~DriverMessenger()
{




}

void DriverMessenger::WriteTracking(TrackingUpdate t)
{
	m_trackingData.Write(t);
}

void DriverMessenger::WriteSuits(SuitsConnectionInfo s)
{
	m_suitConnectionInfo.Write(s);
}

void DriverMessenger::WriteLog(std::string s)
{
	//assumes null termination
	if (!m_loggingStream.Push(s.data(), s.length() + 1)) {
		std::cout << "Failed to push to log stream";
	}
}

boost::optional<std::vector<NullSpaceIPC::EffectCommand>> DriverMessenger::ReadHaptics()
{
	return readFromStream<NullSpaceIPC::EffectCommand, OwnedReadableSharedQueue>(m_hapticsData, 100);
}

boost::optional<std::vector<NullSpaceIPC::DriverCommand>> DriverMessenger::ReadCommands()
{

	return readFromStream<NullSpaceIPC::DriverCommand, OwnedReadableSharedQueue>(m_commandStream, 100);
}




void DriverMessenger::Disconnect()
{
	_running.store(false);
	m_sentinalTimer.cancel();
}


