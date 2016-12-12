#include "stdafx.h"
#include "SuitInfoConsumer.h"



SuitInfoConsumer::SuitInfoConsumer(SuitHardwareInterface::VersionInfoCallback c):_callbackToEngine(c)
{
}


SuitInfoConsumer::~SuitInfoConsumer()
{
}

void SuitInfoConsumer::ConsumePacket(const packet & packet)
{
	unsigned int major = packet.raw[3];
	unsigned int minor = packet.raw[4];
	_callbackToEngine(SuitHardwareInterface::VersionInfo(major, minor));
}
