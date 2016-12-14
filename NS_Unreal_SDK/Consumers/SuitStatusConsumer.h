#pragma once
#include "IPacketConsumer.h"
#include "SuitDiagnostics.h"


class SuitStatusConsumer : public IPacketConsumer
{
public:
	SuitStatusConsumer(SuitDiagnostics::SuitDiagnosticsCallback cb);
	~SuitStatusConsumer();
	void ConsumePacket(const packet& packet) override;
private:
	SuitDiagnostics::SuitDiagnosticsCallback _callback;
};

