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
private:
	std::unordered_map<Imu, Quaternion> _quaternions;
	Quaternion parseQuaternion(const uint8_t* rec) const;
};

