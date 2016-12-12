#pragma once
#include "IPacketConsumer.h"
#include <unordered_map>
#include "boost\optional.hpp"
enum class Imu;
struct Quaternion;
class ImuConsumer : public IPacketConsumer
{
public:
	ImuConsumer();
	~ImuConsumer();
	boost::optional<Quaternion> GetOrientation(Imu imu);
	void ConsumePacket(const packet& packet) override;
	void AssignMapping(uint32_t key, Imu id);
private:
	std::unordered_map<Imu, Quaternion> _quaternions;
	Quaternion parseQuaternion(const uint8_t* rec) const;
	std::unordered_map<uint32_t, Imu> _mapping;
};

