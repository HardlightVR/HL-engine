#pragma once

#include <unordered_map>
#include "boost\optional.hpp"
#include "SharedCommunication/SharedTypes.h"
enum class Imu;
class ImuConsumer 
{
public:
	ImuConsumer();
	~ImuConsumer();
	boost::optional<NullSpace::SharedMemory::Quaternion> GetOrientation(Imu imu);
//	void ConsumePacket(packet packet);
	void AssignMapping(uint32_t key, Imu id);
private:
	std::unordered_map<Imu, NullSpace::SharedMemory::Quaternion> _quaternions;
	NullSpace::SharedMemory::Quaternion parseQuaternion(const uint8_t* rec) const;
	std::unordered_map<uint32_t, Imu> _mapping;
};

