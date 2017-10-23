#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/signals2.hpp>
#include <functional>


class FirmwareInterface;
class Heartbeat
{
public:
	Heartbeat(boost::asio::io_service& io, FirmwareInterface& fi);
	
	void BeginListening();

	// Called to let the Heartbeat know that it has received a response (a ping or potentially any other data that 
	// implies the hardware is connected.)
	void ReceiveResponse();

	bool IsConnected() const;

	using ConnectionHandler = boost::signals2::signal<void()>;

	// Subscribe to disconnect and reconnect events
	void OnDisconnect(ConnectionHandler::slot_type);
	void OnReconnect(ConnectionHandler::slot_type);
private:
	FirmwareInterface& m_firmware;
	
	bool m_isConnected;

	long long m_latestResponseTime;

	unsigned int m_maxFailedPings;

	unsigned int m_currentFailedPings;

	boost::posix_time::milliseconds m_heartbeatInterval;

	boost::asio::deadline_timer m_heartbeatTimer;

	// How long we wait for a response before timing out
	boost::posix_time::milliseconds m_responseTimeout;

	boost::asio::deadline_timer m_responseTimer;
	
	ConnectionHandler m_onConnect;
	ConnectionHandler m_onDisconnect;
	
	

	void doHeartbeat();
	void onReceiveResponse(const boost::system::error_code& ec);

	//Convenience methods to setup and await the timers
	void schedulePingTimer();
	void scheduleResponseTimer();


	void raiseDisconnect();
	void raiseReconnect();

	
};

