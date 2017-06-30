#include "stdafx.h"
#include "ImuConsumer.h"
#include "Enums.h"

#include "PacketDispatcher.h"
ImuConsumer::ImuConsumer(PacketDispatcher &dispatcher):
	m_quaternions(),
	m_mapping()
{
	dispatcher.AddConsumer(SuitPacket::PacketType::ImuData, [&](packet p) {
		this->consumePacket(p);
	});

	dispatcher.AddConsumer(SuitPacket::PacketType::DummyTracking, [&](packet p) {
		this->consumePacketDummy(p);
	});
}

void ImuConsumer::OnTracking(TrackingCallback cb)
{
	m_callback = cb;
}

ImuConsumer::~ImuConsumer()
{
}

void ImuConsumer::AssignMapping(uint32_t key, Imu id, const std::string& readable_id)
{
	m_mapping[key] = std::make_pair(id, readable_id);
}
void ImuConsumer::consumePacket(packet packet)
{

	const auto& mapping = m_mapping[packet.raw[11]];
	Imu id = mapping.first;
	if (id != Imu::Unknown) {
		m_quaternions[id] = parseQuaternion(packet.raw);
		if (m_callback) {
			(*m_callback)(mapping.second, m_quaternions.at(id));
		}
	}
}

void ImuConsumer::consumePacketDummy(packet packet)
{
	const auto& mapping = m_mapping[packet.raw[11]];
	Imu id = mapping.first;
	if (id != Imu::Unknown) {
		float x;
		memcpy(&x, &packet.raw[3], sizeof(x));
		NSVR_Core_Quaternion q = { 0 };
		q.x = x;
		q.w = 1.0f;
		m_quaternions[id] = q;
		if (m_callback) {
			(*m_callback)(mapping.second, m_quaternions.at(id));
		}
	}
}

NSVR_Core_Quaternion ImuConsumer::parseQuaternion(const uint8_t * rec) const
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

	NSVR_Core_Quaternion quat;
	quat.w = q[0];
	quat.x = q[1];
	quat.y = q[2];
	quat.z = q[3];

	return quat;
}


