#pragma once

struct packet;
class IPacketConsumer
{
public:
	virtual void ConsumePacket(const packet& packet) = 0;
	virtual ~IPacketConsumer() {};
};

