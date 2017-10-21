#pragma once

#include <unordered_map>
#include <boost\optional.hpp>
#include "suit_packet.h"
#include "PluginAPI.h"
#include <tuple>
enum class Imu;




class PacketDispatcher;

struct Mapping {
	Imu imu;
	nsvr_node_id node_id;
	nsvr_tracking_stream* stream;
	Mapping() : imu(), node_id(), stream(nullptr) {}
	Mapping(Imu imu, nsvr_node_id node_id) : imu(imu), node_id(node_id), stream(nullptr) {}
};
class ImuConsumer 
{
public:
	ImuConsumer(PacketDispatcher&);
	void AssignMapping(uint32_t key, Imu id, nsvr_node_id node_id);
	void AssignStream(nsvr_tracking_stream* stream, nsvr_node_id id);

	void RemoveStream(nsvr_node_id id);
private:
	void consumePacket(Packet Packet);

	std::unordered_map<Imu, nsvr_quaternion> m_quaternions;
	nsvr_quaternion parseQuaternion(const uint8_t* rec) const;
	std::unordered_map<uint32_t, Mapping> m_mapping;
};

