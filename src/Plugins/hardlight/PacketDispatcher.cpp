#include "stdafx.h"
#include "PacketDispatcher.h"


PacketDispatcher::PacketDispatcher() : _dispatchLimit(32), _consumers()
{
}
void PacketDispatcher::Dispatch(packet packet)
{
	SuitPacket::PacketType packetType = SuitPacket::Type(packet);
	if (_consumers.find(packetType) != _consumers.end())
	{
		for (auto monitor : _consumers.at(packetType))
		{
			monitor(packet);
		}
	}
}

void PacketDispatcher::AddConsumer(SuitPacket::PacketType ptype, OnReceivePacketFunc packetFunc)
{
	_consumers[ptype].push_back(packetFunc);
}

