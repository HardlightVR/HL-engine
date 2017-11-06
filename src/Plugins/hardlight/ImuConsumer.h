#pragma once

#include <unordered_map>
#include "suit_packet.h"
#include "PluginAPI.h"
#include "HL_Firmware_Defines.h"

enum class Imu;
class PacketDispatcher;

struct Mapping {
	Imu imu;
	nsvr_node_id node_id;
	nsvr_tracking_stream* stream;
	HL_Unit status;
	Mapping() : imu(), node_id(), stream(nullptr), status(HL_Unit::None){}
	Mapping(Imu imu, nsvr_node_id node_id) : imu(imu), node_id(node_id), stream(nullptr), status(HL_Unit::None) {}
};

struct ImuInfo {
	Imu friendlyName;
	HL_Unit status;
	uint32_t firmwareId;
};
class ImuConsumer 
{
public:
	ImuConsumer(PacketDispatcher&);
	void AssignMapping(uint32_t key, Imu id, nsvr_node_id node_id);
	void AssignStream(nsvr_tracking_stream* stream, nsvr_node_id id);
	void RemoveStream(nsvr_node_id id);

	std::vector<ImuInfo> GetInfo() const;
private:
	void consumeDataPacket(Packet Packet);
	void ImuConsumer::consumeStatusPacket(Packet packet);
	std::unordered_map<Imu, nsvr_quaternion> m_quaternions;
	nsvr_quaternion parseQuaternion(const uint8_t* rec) const;
	std::unordered_map<uint32_t, Mapping> m_mapping;
};

