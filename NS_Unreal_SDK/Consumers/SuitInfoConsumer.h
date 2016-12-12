#pragma once
#include "IPacketConsumer.h"
#include <functional>
#include "SuitHardwareInterface.h"
class SuitInfoConsumer : public IPacketConsumer
{
public:
	
	SuitInfoConsumer(SuitHardwareInterface::VersionInfoCallback c);
	~SuitInfoConsumer();
	void ConsumePacket(const packet& packet) override;
private:
	SuitHardwareInterface::VersionInfoCallback _callbackToEngine;
};

