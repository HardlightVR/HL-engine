#pragma once

#include <memory>
#include <atomic>

#include "SharedCommunication\WritableSharedObject.h"
#include "SharedCommunication\OwnedReadableSharedQueue.h"
#include "SharedCommunication\OwnedWritableSharedQueue.h"
#include "SharedCommunication\OwnedWritableSharedMap.h"
#include "SharedCommunication\OwnedWritableSharedVector.h"
#include "SharedCommunication\SharedTypes.h"
#include "protobuff_defs/EffectCommand.pb.h"
#include "protobuff_defs/DriverCommand.pb.h"
#include "protobuff_defs/HighLevelEvent.pb.h"
#include "protobuff_defs/DeviceEvent.pb.h"


#include <boost/asio/deadline_timer.hpp>

using namespace NullSpace::SharedMemory;


class DriverMessenger
{
public:
	using DataCallback = std::function<void(void const* data, std::size_t length)>;
	DriverMessenger(boost::asio::io_service& io);
	void WriteTracking(uint32_t, NullSpace::SharedMemory::Quaternion quat);
	void WriteDevice(const DeviceInfo& s);
	void WriteNode(const NodeInfo& node);
	void RemoveNode(uint64_t id);
	void RemoveDevice(uint32_t id);
	void WriteBodyView(NullSpace::SharedMemory::RegionPair data);
	boost::optional<std::vector<NullSpaceIPC::EffectCommand>> ReadHaptics();
	boost::optional<std::vector<NullSpaceIPC::HighLevelEvent>> ReadEvents();
	boost::optional<std::vector<NullSpaceIPC::DriverCommand>> ReadCommands();
	void Disconnect();
private:
	std::function<void(void const* data, std::size_t length)> _process;
	
	//Write tracking data here
	std::unique_ptr<WritableSharedObject<TrackingUpdate>> m_trackingData;



	//Write haptics data here
	std::unique_ptr<OwnedReadableSharedQueue> m_hapticsData;

	std::unique_ptr<OwnedWritableSharedVector<DeviceInfo>> m_devices;

	std::unique_ptr<OwnedWritableSharedVector<NodeInfo>> m_nodes;

	std::unique_ptr<OwnedWritableSharedQueue> m_loggingStream;

	std::unique_ptr<WritableSharedObject<NullSpace::SharedMemory::SentinelObject>> m_sentinel;

	std::unique_ptr<OwnedReadableSharedQueue> m_commandStream;

	std::unique_ptr<OwnedWritableSharedMap<uint32_t, NullSpace::SharedMemory::Quaternion>> m_tracking;

	std::unique_ptr<OwnedWritableSharedVector<NullSpace::SharedMemory::RegionPair>> m_bodyView;
	std::atomic<bool> _running;

	boost::asio::deadline_timer m_sentinelTimer;
	boost::posix_time::milliseconds m_sentinelInterval;

	void sentinelHandler(const boost::system::error_code&);
	void startSentinel();

	template<typename TResult, typename TQueueType>
	std::vector<TResult> readFromStream(TQueueType& queue, std::size_t max_messages);
public:
	void UpdateDeviceStatus(uint32_t id, DeviceStatus status);
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
