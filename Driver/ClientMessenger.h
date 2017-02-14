#pragma once
#include <boost/interprocess/ipc/message_queue.hpp>

#include "SharedCommunication\ReadableSharedQueue.h"
#include "SharedCommunication\ReadableSharedObject.h"
#include "SharedCommunication\WritableSharedQueue.h"
using namespace boost::interprocess;
class ClientMessenger
{
public:
	ClientMessenger(boost::asio::io_service&);
	~ClientMessenger();
	
private:
	//Wite haptics to the suit using this shared queue
	std::unique_ptr<WritableSharedQueue> m_hapticsStream;

	//Read the most up-to-date tracking data from this object
	std::unique_ptr<ReadableSharedObject<int>> m_trackingData;

	//Read the most up-to-date suit connection information from this object
	std::unique_ptr<ReadableSharedObject<int>> m_suitConnectionInfo;

	//Sentinal to see if the driver is running
	std::unique_ptr<ReadableSharedObject<std::time_t>> m_sentinal;


	boost::asio::deadline_timer m_sentinalTimer;
	boost::posix_time::milliseconds m_sentinalInterval;

	boost::chrono::milliseconds m_sentinalTimeout;

	void startAttemptEstablishConnection();
	void attemptEstablishConnection(const boost::system::error_code& ec);

	void startMonitorConnection();
	void monitorConnection(const boost::system::error_code& ec);

};

