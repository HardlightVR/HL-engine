#include "stdafx.h"
#include "ImuConsumer.h"
#include "Enums.h"
#include "PacketDispatcher.h"
ImuConsumer::ImuConsumer(PacketDispatcher &dispatcher):
	m_quaternions(),
	m_mapping()
{
	dispatcher.AddConsumer(inst::Id::GET_TRACK_DATA, [&](Packet p) {
		this->consumeDataPacket(p);
	});

	dispatcher.AddConsumer(inst::Id::GET_TRACK_STATUS, [&](Packet p) {
		this->consumeStatusPacket(p);
	});

	dispatcher.AddConsumer(inst::Id::GET_TRACK_GRAVITY, [&](Packet p) {
		this->consumeGravityPacket(p);
	});

	dispatcher.AddConsumer(inst::Id::GET_TRACK_MAG, [&](Packet p) {
		this->consumeCompassPacket(p);
	});


}

void ImuConsumer::AssignStream(nsvr_tracking_stream* stream, nsvr_node_id id) {
	for (auto& kvp : m_mapping) {
		if (kvp.second.node_id == id) {
			kvp.second.stream = stream;
		}
	}
}


void ImuConsumer::RemoveStream(nsvr_node_id id) {
	for (auto& kvp : m_mapping) {
		if (kvp.second.node_id == id) {
			kvp.second.stream = nullptr;
		}
	}
}

std::vector<ImuInfo> ImuConsumer::GetInfo() const
{
	std::vector<ImuInfo> mappings;
	for (const auto& kvp : m_mapping) {
		mappings.push_back(ImuInfo{ kvp.second.imu, kvp.second.status, kvp.first });
	}
	return mappings;
}

void ImuConsumer::AssignMapping(uint32_t key, Imu id, nsvr_node_id node_id)
{
	m_mapping[key] = Mapping(id, node_id);
}
void ImuConsumer::consumeStatusPacket(Packet packet) {

	auto iter = m_mapping.find(packet[4]);
	if (iter != m_mapping.end()) {
		iter->second.status = static_cast<HL_Unit::_enumerated>(packet[3]);
	}
}
void ImuConsumer::consumeCompassPacket(Packet packet)
{
	auto iter = m_mapping.find(packet[11]);
	if (iter != m_mapping.end()) {
		if (iter->second.stream) {
			nsvr_quaternion q = parseQuaternion(packet.data());
			nsvr_vector3 v{ q.w, q.x, q.y };
			nsvr_tracking_stream_push_compass(iter->second.stream, &v);
		}
	}
}
void ImuConsumer::consumeGravityPacket(Packet packet)
{
	auto iter = m_mapping.find(packet[11]);
	if (iter != m_mapping.end()) {
		if (iter->second.stream) {
			nsvr_quaternion q = parseQuaternion(packet.data());
			nsvr_vector3 v{ q.w, q.x, q.y };
			nsvr_tracking_stream_push_gravity(iter->second.stream, &v);
		}
	}
}
void ImuConsumer::consumeDataPacket(Packet packet)
{

	auto iter = m_mapping.find(packet[11]);
	if (iter != m_mapping.end()) {
		m_quaternions[iter->second.imu] = parseQuaternion(packet.data());

		if (iter->second.stream) {
			nsvr_tracking_stream_push_quaternion(iter->second.stream, &m_quaternions.at(iter->second.imu));
		}
	}
}


nsvr_quaternion ImuConsumer::parseQuaternion(const uint8_t * rec) const
{
	int offset = 3;
	float q[4];
	q[0] = ((rec[0 + offset] << 8) | rec[1 + offset]) / 16384.0f;
	q[1] = ((rec[2 + offset] << 8) | rec[3 + offset]) / 16384.0f;
	q[2] = ((rec[4 + offset] << 8) | rec[5 + offset]) / 16384.0f;
	q[3] = ((rec[6 + offset] << 8) | rec[7 + offset]) / 16384.0f;
	for (int i = 0; i < 4; i++) {
		if (q[i] >= 2) {
			q[i] = -4 + q[i];
		}
	}

	nsvr_quaternion quat;
	quat.w = q[0];
	quat.x = q[1];
	quat.y = q[2];
	quat.z = q[3];

	return quat;
}


