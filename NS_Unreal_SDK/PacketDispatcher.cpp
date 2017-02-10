#include "StdAfx.h"
#include "PacketDispatcher.h"


PacketDispatcher::PacketDispatcher() : _dispatchLimit(32)
{
}


PacketDispatcher::~PacketDispatcher()
{
}

void PacketDispatcher::Dispatch(packet packet)
{
	SuitPacket::PacketType packetType = SuitPacket::Type(packet);
	
	if (_consumers.find(packetType) != _consumers.end())
	{
		for (auto monitor : _consumers.at(packetType))
		{
			monitor->ConsumePacket(packet);
		}
	}
}

void PacketDispatcher::AddConsumer(SuitPacket::PacketType ptype, IPacketConsumer* consumer)
{
	_consumers[ptype].push_back(consumer);
	
}
