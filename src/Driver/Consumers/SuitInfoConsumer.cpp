#include "stdafx.h"
#include "SuitInfoConsumer.h"



SuitInfoConsumer::SuitInfoConsumer(SuitDiagnostics::SuitVersionCallback c):_callback(c)
{
}


SuitInfoConsumer::~SuitInfoConsumer()
{
}

void SuitInfoConsumer::ConsumePacket(const packet & packet)
{
	unsigned int major = packet.raw[3];
	unsigned int minor = packet.raw[4];
	_callback(SuitDiagnostics::VersionInfo(major, minor));
}
