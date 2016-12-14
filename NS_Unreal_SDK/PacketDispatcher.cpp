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
	SuitPacket p(packet);
	auto packetType = p.Type();
	
	if (_consumers.find(packetType) != _consumers.end())
	{
		for (auto monitor : _consumers.at(packetType))
		{
			monitor->ConsumePacket(packet);
		}
	}
}

void PacketDispatcher::AddConsumer(SuitPacket::PacketType ptype, std::shared_ptr<IPacketConsumer> consumer)
{
	_consumers[ptype].push_back(consumer);
	//if (_consumers.find(ptype) != _consumers.end()) {
	//	_consumers[ptype].push_back(consumer);
	//}
	//else {
	//	_consumers[ptype] = std::vector<std::shared_ptr<IPacketConsumer>>();
	//	_consumers[ptype].push_back(consumer);

	//}
}
