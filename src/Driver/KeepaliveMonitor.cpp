#include "stdafx.h"
#include "KeepaliveMonitor.h"
#include "Locator.h"
#include <boost\log\trivial.hpp>
#include <boost\asio\serial_port.hpp>
#include "FirmwareInterface.h"
KeepaliveMonitor::KeepaliveMonitor(boost::asio::io_service& io, FirmwareInterface& fi):
	m_fi(fi),
	_responseTimer(io), 
	_responseTimeout(boost::posix_time::milliseconds(3000)),
	_pingTimer(io),
	_pingInterval(boost::posix_time::milliseconds(500)),
	MAX_FAILED_PINGS(2),
	_failedPingCount(0),
	_pingTime(0)
	
{

}



KeepaliveMonitor::~KeepaliveMonitor()
{
	
}

void KeepaliveMonitor::schedulePingTimer()
{
	_pingTimer.expires_from_now(_pingInterval);
	_pingTimer.async_wait([&](auto& ec) { doKeepAlivePing(); });
}

void KeepaliveMonitor::scheduleResponseTimer()
{
	_responseTimer.expires_from_now(_responseTimeout);
	_responseTimer.async_wait([&](auto& ec) { onReceiveResponse(ec); });
}


void KeepaliveMonitor::SetDisconnectHandler(std::function<void()> handler)
{
	_disconnectHandler = handler;
}

void KeepaliveMonitor::ReceivePing()
{
	
	_responseTimer.cancel();
}

void KeepaliveMonitor::BeginMonitoring()
{
	schedulePingTimer();
}



void KeepaliveMonitor::SetMaxAllowedResponseTime(boost::posix_time::millisec max)
{
	_responseTimeout = max;
}

void KeepaliveMonitor::doKeepAlivePing()
{
	m_fi.Ping();
	
	scheduleResponseTimer();
	
}

void KeepaliveMonitor::onReceiveResponse(const boost::system::error_code& ping_recd)
{


	if (ping_recd) {
		//Timer was canceled - this is the most common execution of this function.
		//Most likely triggered by the adapter seeing a ping response and calling ReceivePing()
	
		//store ping-response time for aid in debugging and logging
		_pingTime = _responseTimeout.total_milliseconds() - _responseTimer.expires_from_now().total_milliseconds();
		std::cout << "The ping time from send to receive was " << _pingTime << " ms\n";
		schedulePingTimer();
	}
	else {
		_disconnectHandler();
	}

}

