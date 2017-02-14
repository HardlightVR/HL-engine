#include "stdafx.h"
#include "DriverMessenger.h"


DriverMessenger::DriverMessenger(boost::asio::io_service& io):
_running{true},
	_process([](void const*, std::size_t) {}),
	m_hapticsData("ns-haptics-data", 100, 256),
	m_trackingData("ns-tracking-data"),
	m_suitConnectionInfo("ns-suit-data"),
	m_sentinal("ns-sentinal"),

	m_sentinalTimer(io),
	m_sentinalInterval(1000)

{
	
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




void DriverMessenger::Disconnect()
{
	_running.store(false);
	m_sentinalTimer.cancel();
}


