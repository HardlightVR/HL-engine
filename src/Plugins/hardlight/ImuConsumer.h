#pragma once

#include <unordered_map>
#include <boost\optional.hpp>
#include "suit_packet.h"
#include "PluginAPI.h"
enum class Imu;




class PacketDispatcher;

class ImuConsumer 
{
public:
	using TrackingCallback = std::function<void(const std::string&, NSVR_Core_Quaternion)>;
	ImuConsumer(PacketDispatcher&);
	void OnTracking(TrackingCallback);
	~ImuConsumer();
	void AssignMapping(uint32_t key, Imu id, const std::string& readable_id);
private:
	boost::optional<TrackingCallback> m_callback;
	void consumePacket(packet packet);
	void consumePacketDummy(packet packet);

	std::unordered_map<Imu, NSVR_Core_Quaternion> m_quaternions;
	NSVR_Core_Quaternion parseQuaternion(const uint8_t* rec) const;
	std::unordered_map<uint32_t, std::pair<Imu, std::string>> m_mapping;
};

