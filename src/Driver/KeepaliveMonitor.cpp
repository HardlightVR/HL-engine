#include "stdafx.h"
#include "KeepaliveMonitor.h"
#include "Locator.h"
#include <boost\log\trivial.hpp>
#include <boost\asio\serial_port.hpp>
#include "FirmwareInterface.h"
KeepaliveMonitor::KeepaliveMonitor(boost::asio::io_service& io, FirmwareInterface& fi):
	m_fi(fi),
	_responseTimer(io), 
	_responseTimeout(boost::posix_time::milliseconds(100)),
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
	_pingTimer.async_wait(boost::bind(&KeepaliveMonitor::doKeepAlivePing, this));
}

void KeepaliveMonitor::scheduleResponseTimer()
{
	_responseTimer.expires_from_now(_responseTimeout);
	_responseTimer.async_wait(boost::bind(&KeepaliveMonitor::onReceiveResponse, this, boost::asio::placeholders::error));
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
	//scheduleResponseTimer();
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

void KeepaliveMonitor::onReceiveResponse(const boost::system::error_code& ec)
{
	assert(_failedPingCount <= MAX_FAILED_PINGS);


	if (ec) {
		//Timer was canceled - this is the most common execution of this function.
		//Most likely triggered by the adapter seeing a ping response and calling ReceivePing()
	
		//Since we are on the normal path, set the _failedPingCount to 0
		_failedPingCount = 0;

		//store ping-response time for aid in debugging and logging
		_pingTime = _responseTimeout.total_milliseconds() - _responseTimer.expires_from_now().total_milliseconds();

		schedulePingTimer();
		return;
	}
	else {
		//This block only happens if the timer is allowed to timeout. It only times out
		//if it failed to receive a ping. 
		_failedPingCount++;

		if (_failedPingCount >= MAX_FAILED_PINGS) {
			BOOST_LOG_TRIVIAL(info) << "[KeepAlive] After " << _failedPingCount << " failed pings, notifying suit disconnection";

			_failedPingCount = 0;
			_disconnectHandler();
		}
		else {
			schedulePingTimer();
		}
	}

}

