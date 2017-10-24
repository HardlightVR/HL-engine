#include "stdafx.h"
#include "Heartbeat.h"
#include "FirmwareInterface.h"
#include "logger.h"

Heartbeat::Heartbeat(boost::asio::io_service& io, FirmwareInterface& fi)
	: m_firmware(fi)
	, m_isConnected(false)
	, m_latestResponseTime(0)
	, m_maxFailedPings(3)
	, m_currentFailedPings(0)
	, m_heartbeatInterval(boost::posix_time::milliseconds(500))
	, m_heartbeatTimer(io)
	, m_responseTimeout(boost::posix_time::milliseconds(500))
	, m_responseTimer(io)
	, m_onConnect()
	, m_onDisconnect()
{
}


void Heartbeat::BeginListening()
{
	schedulePingTimer();
}

void Heartbeat::schedulePingTimer()
{
	m_heartbeatTimer.expires_from_now(m_heartbeatInterval);
	m_heartbeatTimer.async_wait([&](auto& ec) { doHeartbeat(); });
}


void Heartbeat::doHeartbeat()
{
	m_firmware.Ping();
	int a = 9;
	scheduleResponseTimer();
}


void Heartbeat::scheduleResponseTimer()
{
	m_responseTimer.expires_from_now(m_responseTimeout);
	m_responseTimer.async_wait([&](auto& ec) { onReceiveResponse(ec); });
}

void Heartbeat::onReceiveResponse(const boost::system::error_code& ping_received)
{
	if (ping_received) {
		m_currentFailedPings = 0;
		
		if (!m_isConnected) {
			m_isConnected = true;
			raiseReconnect();
		}

		m_latestResponseTime = m_responseTimeout.total_milliseconds() - m_responseTimer.expires_from_now().total_milliseconds();
		
		schedulePingTimer();
		core_log("Heartbeat", std::string("Last response time: " + std::to_string(m_latestResponseTime)));
	}

	else {
		m_currentFailedPings++;
		
		std::stringstream fmt;
		fmt << "Bad ping [" << m_currentFailedPings << "/" << m_maxFailedPings << "]";
		core_log("Heartbeat", fmt.str());

		if (m_currentFailedPings < m_maxFailedPings) {
			schedulePingTimer();
		} else {
			m_isConnected = false;
			m_currentFailedPings = 0;
			raiseDisconnect();
		}
	}
}

void Heartbeat::raiseReconnect()
{
	m_onConnect();
}

void Heartbeat::raiseDisconnect()
{
	m_onDisconnect();
}

void Heartbeat::ReceiveResponse()
{
	m_responseTimer.cancel();
}

bool Heartbeat::IsConnected() const
{
	return m_isConnected;
}

void Heartbeat::OnDisconnect(Heartbeat::ConnectionHandler::slot_type handler)
{
	m_onDisconnect.connect(handler);
}

void Heartbeat::OnReconnect(Heartbeat::ConnectionHandler::slot_type handler)
{
	m_onConnect.connect(handler);
}










