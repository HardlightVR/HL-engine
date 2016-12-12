#pragma once
#include "IPacketConsumer.h"
class SuitStatusConsumer : public IPacketConsumer
{
public:
	SuitStatusConsumer();
	~SuitStatusConsumer();
	void ConsumePacket(const packet& packet) override;
};

