#include "stdafx.h"
#include "ImuConsumer.h"
#include "Enums.h"

#include "PacketDispatcher.h"
ImuConsumer::ImuConsumer(PacketDispatcher &dispatcher):
	m_quaternions(),
	m_mapping()
{
	dispatcher.AddConsumer(PacketType::ImuData, [&](Packet p) {
		this->consumePacket(p);
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

void ImuConsumer::AssignMapping(uint32_t key, Imu id, nsvr_node_id node_id)
{
	m_mapping[key] = Mapping(id, node_id);
}
void ImuConsumer::consumePacket(Packet Packet)
{

	const auto& mapping = m_mapping[Packet[11]];
	Imu id = mapping.imu;
	if (id != Imu::Unknown) {
		m_quaternions[id] = parseQuaternion(Packet.data());

		if (mapping.stream) {
			nsvr_tracking_stream_push(mapping.stream, &m_quaternions.at(id));
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


