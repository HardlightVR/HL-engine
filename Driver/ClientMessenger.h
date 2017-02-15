#pragma once
#include <boost/interprocess/ipc/message_queue.hpp>

#include "SharedCommunication\ReadableSharedQueue.h"
#include "SharedCommunication\ReadableSharedObject.h"
#include "SharedCommunication\WritableSharedQueue.h"
#include "SharedCommunication\SharedTypes.h"
#include "Encoder.h"

using namespace boost::interprocess;
using namespace NullSpace::SharedMemory;
class ClientMessenger
{
public:
	ClientMessenger(boost::asio::io_service&);
	~ClientMessenger();
	boost::optional<TrackingUpdate> ReadTracking();
	boost::optional<SuitsConnectionInfo> ReadSuits();
	void WriteHaptics(ExecutionCommand e);

private:
	//Wite haptics to the suit using this shared queue
	std::unique_ptr<WritableSharedQueue> m_hapticsStream;

	//Read the most up-to-date tracking data from this object
	std::unique_ptr<ReadableSharedObject<TrackingUpdate>> m_trackingData;

	//Read the most up-to-date suit connection information from this object
	std::unique_ptr<ReadableSharedObject<SuitsConnectionInfo>> m_suitConnectionInfo;

	//Get logging info from engine. Note: only one consumer can reliably get the debug info
	std::unique_ptr<ReadableSharedQueue> m_logStream;

	//Sentinal to see if the driver is running
	std::unique_ptr<ReadableSharedObject<std::time_t>> m_sentinal;


	//We use a sentinal to see if the driver is responsive/exists
	boost::asio::deadline_timer m_sentinalTimer;

	//How often we read the sentinal
	boost::posix_time::milliseconds m_sentinalInterval;

	//If currentTime - sentinalTime > m_sentinalTimeout, we say that we are disconnected
	boost::chrono::milliseconds m_sentinalTimeout;



	void startAttemptEstablishConnection();
	void attemptEstablishConnection(const boost::system::error_code& ec);

	void startMonitorConnection();
	void monitorConnection(const boost::system::error_code& ec);

	Encoder m_encoder;
};

