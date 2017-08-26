#include "stdafx.h"
#include "KeepaliveMonitor.h"
#include <boost\log\trivial.hpp> 
#include "FirmwareInterface.h"

KeepaliveMonitor::KeepaliveMonitor(boost::asio::io_service& io, FirmwareInterface& fi):
	m_firmware(fi),
	
	m_responseTimer(io), 
	m_responseTimeout(boost::posix_time::milliseconds(1000)),
	
	m_pingTimer(io),
	m_pingInterval(boost::posix_time::milliseconds(500)),
	
	m_lastestPingTime(0),
	m_isConnected(false)
	
{

}


void KeepaliveMonitor::BeginMonitoring()
{
	schedulePingTimer();
}

void KeepaliveMonitor::schedulePingTimer()
{
	m_pingTimer.expires_from_now(m_pingInterval);
	m_pingTimer.async_wait([&](auto& ec) { doKeepAlivePing(); });
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

		if (!m_isConnected) {
			raiseReconnect();
			m_isConnected = true;
		}

		m_lastestPingTime = m_responseTimeout.total_milliseconds() 
			- m_responseTimer.expires_from_now().total_milliseconds();
		schedulePingTimer();
	}
	else {
		m_isConnected = false;
		raiseDisconnect();
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

void KeepaliveMonitor::OnDisconnect(std::function<void()> handler)
{
	m_disconnectHandlers.push_back(std::move(handler));
}

void KeepaliveMonitor::OnReconnect(std::function<void()> handler)
{
	m_reconnectHandlers.push_back(std::move(handler));
}










