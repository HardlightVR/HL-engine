#pragma once

#include "SharedCommunication\WritableSharedObject.h"
#include "SharedCommunication\OwnedReadableSharedQueue.h"
struct ExecutionCommand {
	int Location;
	int Effect;
	short Command;
};


using namespace boost::interprocess;
class DriverMessenger
{
public:
	DriverMessenger(boost::asio::io_service& io);
	~DriverMessenger();
	void Receive(const std::function<void(void const* data, std::size_t length)>);
	bool Poll(const std::function<void(void const * data, std::size_t length)>&);
	void Disconnect();
private:
	std::function<void(void const* data, std::size_t length)> _process;
	
	//Write tracking data here
	WritableSharedObject<int> m_trackingData;

	//Write suit connection data here (which suits connected, etc)
	WritableSharedObject<int> m_suitConnectionInfo;

	//Write haptics data here
	OwnedReadableSharedQueue m_hapticsData;

	//Write a timestamp here every so often to signify that this driver is alive
	WritableSharedObject<std::time_t> m_sentinal;

	std::atomic<bool> _running;

	boost::asio::deadline_timer m_sentinalTimer;
	boost::posix_time::milliseconds m_sentinalInterval;

	void sentinalHandler(const boost::system::error_code&);
	void startSentinal();
};


