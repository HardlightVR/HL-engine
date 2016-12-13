#include "StdAfx.h"
#include "SuitStatusConsumer.h"
#include "SuitDiagnostics.h"





SuitStatusConsumer::SuitStatusConsumer(SuitDiagnostics::SuitDiagnosticsCallback cb):_callback(cb)
{
}

SuitStatusConsumer::~SuitStatusConsumer()
{
}

void SuitStatusConsumer::ConsumePacket(const packet & packet)
{
	SuitDiagnostics::SuitDiagnosticInfo info;
	info.Count = packet.raw[3];
	info.Message = packet.raw[4];
	info.Device = SuitDiagnostics::SuitDiagnosticInfo::DeviceType(packet.raw[5]);
	info.Response = packet.raw[6];
	info.Param1 = packet.raw[7];
	info.Param2 = packet.raw[8];
	_callback(info);

	
}
