#include "stdafx.h"
#include "KeepaliveMonitor.h"
#include "Locator.h"
//not necessarily using the port's io_service
KeepaliveMonitor::KeepaliveMonitor(boost::asio::io_service& io, std::unique_ptr<boost::asio::serial_port>& port) :
	_port(port),
	_responseTimer(io), 
	_responseTimeout(boost::posix_time::milliseconds(20)),
	_pingTimer(io),
	_pingInterval(boost::posix_time::milliseconds(150)),
	_MAX_FAILED_PINGS(2)
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
	scheduleResponseTimer();
}

void KeepaliveMonitor::doKeepAlivePing()
{
	if (_port && _port->is_open()) {
		//only thing special about this packet is the packet type, which is set to ping
		auto pingData = std::make_shared<uint8_t*>(new uint8_t[7]{ 0x24, 0x02, 0x02, 0x07, 0xFF, 0xFF, 0x0A });

		_port->async_write_some(boost::asio::buffer(*pingData, 7), 
			//pass ping data pointer by value to the lambda to ensure it 
			//sticks around as long as boost is accessing it
			[pingData](const boost::system::error_code ec, const std::size_t bytes_transferred) {
				if (ec) {
					//failed to write a ping
					//this happens up to _MAX_FAILED_PINGS times
					Locator::Logger().Log("Keepalive", "Failed to write ping!", LogLevel::Info);
				}
			}
		);

		//Schedule a read - even if the write fails, it could be temporary and we may not need
		//to perform the reset dance
		scheduleResponseTimer();
	}
}

void KeepaliveMonitor::onReceiveResponse(const boost::system::error_code& ec)
{
	assert(_failedPingCount <= _MAX_FAILED_PINGS);


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

		if (_failedPingCount >= _MAX_FAILED_PINGS) {
			_failedPingCount = 0;
			Locator::Logger().Log("Keepalive", "The suit is disconnected", LogLevel::Info);
			_disconnectHandler();
		}
		else {
			schedulePingTimer();
		}
	}

}

