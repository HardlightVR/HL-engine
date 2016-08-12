#pragma once

struct packet;
class IPacketConsumer
{
public:
	virtual void ConsumePacket(packet packet) = 0;
	virtual ~IPacketConsumer() {};
};

