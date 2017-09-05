#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <functional>


class FirmwareInterface;
class KeepaliveMonitor
{
public:
	KeepaliveMonitor(boost::asio::io_service& io, FirmwareInterface& fi);
	// Tell monitor to begin pinging and waiting for responses
	void BeginMonitoring();

	// Inform the monitor that ping data has been received
	void ReceivePing();


	// Subscribe to disconnect and reconnect events
	void OnDisconnect(std::function<void()>);
	void OnReconnect(std::function<void()>);
private:
	FirmwareInterface& m_firmware;
	
	bool m_isConnected;

	// Ping time, updated whenever we receive a new ping
	long long m_lastestPingTime;

	// How long we wait between sending pings
	boost::posix_time::milliseconds m_pingInterval;

	// Timer responsible for firing ping events
	boost::asio::deadline_timer m_pingTimer;

	// How long we wait for a response before timing out
	boost::posix_time::milliseconds m_responseTimeout;

	// Timer for handling response timeout
	boost::asio::deadline_timer m_responseTimer;
	
	std::vector<std::function<void()>> m_disconnectHandlers;
	std::vector<std::function<void()>> m_reconnectHandlers;

	// Max amount of failed pings before raising a disconnect
	unsigned int m_maxFailedPings;

	// Current amount of failed pings
	unsigned int m_currentFailedPings;

	void doKeepAlivePing();
	void onReceiveResponse(const boost::system::error_code& ec);

	//Convenience methods to setup and await the timers
	void schedulePingTimer();
	void scheduleResponseTimer();


	void raiseDisconnect();
	void raiseReconnect();

	/**
	Explanation of ping-response scheme
	===================================

	p = ping
	r = response
	** = _pingInterval
	- = waiting for response

	p--r 
	    **pr
		    **p---r
		           **p--r
	|___|___|___|___|___|___|___| etc..
	t0  t1  t2.. 

	Actual observed ping interval is >= m_pingInterval, which is only lower bound.
	A new ping is only sent out once a response has been received, or we reach m_responseTimeout.
	*/
};

