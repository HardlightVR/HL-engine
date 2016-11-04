#pragma once
#include "IPacketConsumer.h"
class FifoConsumer : public IPacketConsumer
{
public:
	FifoConsumer();
	~FifoConsumer();
	void ConsumePacket(const packet& packet) override;
};

