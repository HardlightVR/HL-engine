#pragma once
#include "IPacketConsumer.h"
#include <functional>
#include "SuitDiagnostics.h"
class SuitInfoConsumer : public IPacketConsumer
{
public:
	
	SuitInfoConsumer(SuitDiagnostics::SuitVersionCallback c);
	~SuitInfoConsumer();
	void ConsumePacket(const packet& packet) override;
private:
	SuitDiagnostics::SuitVersionCallback _callback;
};

