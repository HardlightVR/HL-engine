#include "stdafx.h"
#include "PacketDispatcher.h"


PacketDispatcher::PacketDispatcher() : _dispatchLimit(32), _consumers()
{
}
void PacketDispatcher::Dispatch(packet packet)
{
	PacketType packetType = GetType(packet);
	if (_consumers.find(packetType) != _consumers.end())
	{
		for (auto monitor : _consumers.at(packetType))
		{
			monitor(packet);
		}
	}
}

void PacketDispatcher::AddConsumer(PacketType ptype, OnReceivePacketFunc packetFunc)
{
	_consumers[ptype].push_back(packetFunc);
}

