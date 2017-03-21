#pragma once
class KeepaliveMonitor
{
public:
	KeepaliveMonitor(boost::asio::io_service& io,std::unique_ptr<boost::asio::serial_port>& port);
	~KeepaliveMonitor();

	//Set what happens when the monitor detects a suit disconnect
	void SetDisconnectHandler(std::function<void()>);

	//Tell the monitor that ping data has been received
	void ReceivePing();

	//Tell monitor to begin pinging and waiting for responses
	void BeginMonitoring();

private:
	//keep track of how many pings sent after initial connection loss
	int _failedPingCount;

	//instantaneous ping time, updated whenever receive a new ping
	long long _pingTime;

	//total allowed ping failures before issuing a disconnect
	const std::size_t MAX_FAILED_PINGS;

	//Function which is called when monitor detects a disconnect
	std::function<void()> _disconnectHandler;

	//The port which we are operating on
	std::unique_ptr<boost::asio::serial_port>& _port;
	
	//How long we wait for a response before timing out
	boost::posix_time::milliseconds _responseTimeout;

	//Timer for handling response timeout
	boost::asio::deadline_timer _responseTimer;

	//Minimum wait time before a ping
	boost::posix_time::milliseconds _pingInterval;

	//Timer for handling pings
	boost::asio::deadline_timer _pingTimer;
	
	void doKeepAlivePing();
	void onReceiveResponse(const boost::system::error_code& ec);

	//Convenience methods to setup and await the timers
	void schedulePingTimer();
	void scheduleResponseTimer();


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

	Actual real ping interval is >= _pingInterval, which is a lower bound.
	A new ping is only sent out once a response has been received, or we reach _responseTimeout.
	*/
};

