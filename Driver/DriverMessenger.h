#pragma once

#include "SharedCommunication\WritableSharedObject.h"
#include "SharedCommunication\OwnedReadableSharedQueue.h"
#include "SharedCommunication\OwnedWritableSharedQueue.h"
#include "SharedCommunication\SharedTypes.h"


using namespace boost::interprocess;
using namespace NullSpace::SharedMemory;

class DriverMessenger
{
public:
	typedef std::function<void(void const* data, std::size_t length)> DataCallback;
	DriverMessenger(boost::asio::io_service& io);
	~DriverMessenger();
	void WriteTracking(TrackingUpdate t);
	void WriteSuits(SuitsConnectionInfo s);
	boost::optional<ExecutionCommand> ReadHaptics();
	void Disconnect();
private:
	std::function<void(void const* data, std::size_t length)> _process;
	
	//Write tracking data here
	WritableSharedObject<TrackingUpdate> m_trackingData;

	//Write suit connection data here (which suits connected, etc)
	WritableSharedObject<SuitsConnectionInfo> m_suitConnectionInfo;

	//Write haptics data here
	OwnedReadableSharedQueue m_hapticsData;

	//If logging, write data here
	OwnedWritableSharedQueue m_loggingStream;

	//Write a timestamp here every so often to signify that this driver is alive
	WritableSharedObject<std::time_t> m_sentinal;

	std::atomic<bool> _running;

	boost::asio::deadline_timer m_sentinalTimer;
	boost::posix_time::milliseconds m_sentinalInterval;

	void sentinalHandler(const boost::system::error_code&);
	void startSentinal();
};


