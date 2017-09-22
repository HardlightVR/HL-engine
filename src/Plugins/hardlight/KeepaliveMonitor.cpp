#include "stdafx.h"
#include "KeepaliveMonitor.h"
#include "FirmwareInterface.h"
#include "logger.h"
KeepaliveMonitor::KeepaliveMonitor(boost::asio::io_service& io, FirmwareInterface& fi):
	m_firmware(fi),
	
	m_responseTimer(io), 
	m_responseTimeout(boost::posix_time::milliseconds(500)),
	
	m_writeTimer(io),
	m_pingInterval(boost::posix_time::milliseconds(500)),
	
	m_lastestPingTime(0),
	m_isConnected(false),
	m_maxFailedPings(3),
	m_currentFailedPings(0)
	
{

}


void KeepaliveMonitor::BeginMonitoring()
{
	schedulePingTimer();
}

void KeepaliveMonitor::schedulePingTimer()
{
	m_writeTimer.expires_from_now(m_pingInterval);
	m_writeTimer.async_wait([&](auto& ec) { doKeepAlivePing(); });
}


void KeepaliveMonitor::doKeepAlivePing()
{
	m_firmware.Ping();

	scheduleResponseTimer();
}


void KeepaliveMonitor::scheduleResponseTimer()
{
	m_responseTimer.expires_from_now(m_responseTimeout);
	m_responseTimer.async_wait([&](auto& ec) { onReceiveResponse(ec); });
}

void KeepaliveMonitor::onReceiveResponse(const boost::system::error_code& ping_received)
{
	if (ping_received) {
		m_currentFailedPings = 0;
		if (!m_isConnected) {
			raiseReconnect();
			m_isConnected = true;
		}

		m_lastestPingTime = m_responseTimeout.total_milliseconds() 
			- m_responseTimer.expires_from_now().total_milliseconds();
		schedulePingTimer();

		core_log("KeepAlive", std::string("Last ping time: " + std::to_string(m_lastestPingTime)));
	}
	else {
		m_currentFailedPings++;
		std::stringstream fmt;
		fmt << "Bad ping [" << m_currentFailedPings << "/" << m_maxFailedPings << "]";
		core_log("KeepAlive", fmt.str());

		if (m_currentFailedPings < m_maxFailedPings) {
			schedulePingTimer();
		}else  {
			m_isConnected = false;
			m_currentFailedPings = 0;
			raiseDisconnect();
		}
	}
}

void KeepaliveMonitor::raiseReconnect()
{
	for (const auto& handler : m_reconnectHandlers) {
		handler();
	}
}

void KeepaliveMonitor::raiseDisconnect()
{
	for (const auto& handler : m_disconnectHandlers) {
		handler();
	}
}

void KeepaliveMonitor::ReceivePing()
{
	m_responseTimer.cancel();
}

bool KeepaliveMonitor::IsConnected() const
{
	return m_isConnected;
}

void KeepaliveMonitor::OnDisconnect(std::function<void()> handler)
{
	m_disconnectHandlers.push_back(std::move(handler));
}

void KeepaliveMonitor::OnReconnect(std::function<void()> handler)
{
	m_reconnectHandlers.push_back(std::move(handler));
}










