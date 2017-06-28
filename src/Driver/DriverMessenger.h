#pragma once

#include "SharedCommunication\WritableSharedObject.h"
#include "SharedCommunication\OwnedReadableSharedQueue.h"
#include "SharedCommunication\OwnedWritableSharedQueue.h"
#include "SharedCommunication\OwnedWritableSharedTracking.h"
#include "SharedCommunication\SharedTypes.h"
#include "protobuff_defs/EffectCommand.pb.h"
#include "protobuff_defs/DriverCommand.pb.h"
#include "protobuff_defs/HighLevelEvent.pb.h"

#include <memory>

using namespace NullSpace::SharedMemory;

class DriverMessenger
{
public:
	typedef std::function<void(void const* data, std::size_t length)> DataCallback;
	DriverMessenger(boost::asio::io_service& io);
	~DriverMessenger();
	void WriteTracking(TrackingUpdate t);
	void WriteSuits(SuitsConnectionInfo s);
	
	void WriteLog(std::string s);
	boost::optional<std::vector<NullSpaceIPC::EffectCommand>> ReadHaptics();
	boost::optional<std::vector<NullSpaceIPC::HighLevelEvent>> ReadEvents();
	boost::optional<std::vector<NullSpaceIPC::DriverCommand>> ReadCommands();
	void Disconnect();
private:
	std::function<void(void const* data, std::size_t length)> _process;
	
	//Write tracking data here
	std::unique_ptr<WritableSharedObject<TrackingUpdate>> m_trackingData;

	//Write suit connection data here (which suits connected, etc)
	std::unique_ptr<WritableSharedObject<SuitsConnectionInfo>> m_suitConnectionInfo;

	//Write haptics data here
	std::unique_ptr<OwnedReadableSharedQueue> m_hapticsData;

	//If logging, write data here
	std::unique_ptr<OwnedWritableSharedQueue> m_loggingStream;

	//Write a timestamp here every so often to signify that this driver is alive
	std::unique_ptr<WritableSharedObject<std::time_t>> m_sentinel;

	//Read commands from here, such as ENABLE_TRACKING, DISABLE_TRACKING
	std::unique_ptr<OwnedReadableSharedQueue> m_commandStream;

	std::unique_ptr<OwnedWritableSharedTracking> m_tracking;
	std::atomic<bool> _running;

	boost::asio::deadline_timer m_sentinelTimer;
	boost::posix_time::milliseconds m_sentinelInterval;

	void sentinelHandler(const boost::system::error_code&);
	void startSentinel();

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
