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
	using TrackingCallback = std::function<void(const std::string&, nsvr_quaternion)>;
	ImuConsumer(PacketDispatcher&);
	void OnTracking(TrackingCallback);
	void AssignMapping(uint32_t key, Imu id, const std::string& readable_id);
private:
	boost::optional<TrackingCallback> m_callback;
	void consumePacket(Packet Packet);
	void consumePacketDummy(Packet Packet);

	std::unordered_map<Imu, nsvr_quaternion> m_quaternions;
	nsvr_quaternion parseQuaternion(const uint8_t* rec) const;
	std::unordered_map<uint32_t, std::pair<Imu, std::string>> m_mapping;
};

