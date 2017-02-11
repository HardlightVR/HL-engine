#include "StdAfx.h"
#include "ImuConsumer.h"
#include "Synchronizer.h"
#include "Enums.h"
#include "EncodingOperations.h"

ImuConsumer::ImuConsumer()
{
}


ImuConsumer::~ImuConsumer()
{
}

boost::optional<Quaternion> ImuConsumer::GetOrientation(Imu imu)
{
	try {
		if (_quaternions.find(imu) != _quaternions.end()) {
			return _quaternions.at(imu);
		}
		else {
			return boost::none;
		}
	}
	catch (std::exception&) {
		std::cout << "EXCEPTION while attempting to get quaternion data!\n";
		return boost::none;
	}
}

void ImuConsumer::ConsumePacket(const packet& packet)
{
	//std::cout << "Got Imu  packet: " << int(packet.raw[13]) << " in the fifo" << '\n';

	Imu id = _mapping[packet.raw[11]];
	//if (id == Imu::Chest) {
		//auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - _lastTime);
		//_lastTime = std::chrono::high_resolution_clock::now();
		//std::cout << "Time for left upper arm: " << now.count() << '\n';
	//}

	if (id != Imu::Unknown) {
		_quaternions[id] = parseQuaternion(packet.raw);
	}

}

void ImuConsumer::AssignMapping(uint32_t key, Imu id)
{
	_mapping[key] = id;
}

Quaternion ImuConsumer::parseQuaternion(const uint8_t * rec) const
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
	return Quaternion(q[1], q[2], q[3], q[0]);
}
