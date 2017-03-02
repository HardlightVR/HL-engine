#pragma once

#include "SharedCommunication\WritableSharedObject.h"
#include "SharedCommunication\OwnedReadableSharedQueue.h"
#include "SharedCommunication\OwnedWritableSharedQueue.h"
#include "SharedCommunication\SharedTypes.h"
#include "EffectCommand.pb.h"
#include "DriverCommand.pb.h"

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
	boost::optional<std::vector<NullSpaceIPC::EffectCommand>> ReadHaptics();
	boost::optional<std::vector<NullSpaceIPC::DriverCommand>> ReadCommands();
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

	//Read commands from here, such as ENABLE_TRACKING, DISABLE_TRACKING
	OwnedReadableSharedQueue m_commandStream;

	std::atomic<bool> _running;

	boost::asio::deadline_timer m_sentinalTimer;
	boost::posix_time::milliseconds m_sentinalInterval;

	void sentinalHandler(const boost::system::error_code&);
	void startSentinal();

	template<typename TResult, typename TQueueType>
	std::vector<TResult> readFromStream(TQueueType& queue, std::size_t max_messages);
};

template<typename TResult, typename TQueueType>
inline std::vector<TResult> DriverMessenger::readFromStream(TQueueType& queue, std::size_t max_messages)
{
	std::vector<TResult> commands;
	std::size_t numMsgs = queue.GetNumMessageAvailable();
	std::size_t toRead = std::min<std::size_t>(numMsgs, max_messages);
	for (std::size_t i = 0; i < toRead; i++) {
		if (auto data = queue.Pop()) {
			TResult command;
			if (command.ParseFromArray(data->data(), data->size())) {
				commands.push_back(command);
			}
		}
		else {
			break;
		}
	}
	return commands;
}
