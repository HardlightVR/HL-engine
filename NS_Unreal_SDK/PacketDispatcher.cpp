#include "PacketDispatcher.h"


PacketDispatcher::PacketDispatcher(std::shared_ptr<CircularBuffer> data) : _dispatchLimit(32)
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
		for (auto monitor : _consumers[packetType])
		{
			monitor->ConsumePacket(packet);
		}
	}
}

void PacketDispatcher::AddConsumer(SuitPacket::PacketType ptype, std::shared_ptr<IPacketConsumer> consumer)
{
	if (_consumers.find(ptype) != _consumers.end()) {
		_consumers[ptype].push_back(consumer);
	}
	else {
		_consumers[ptype] = std::vector<std::shared_ptr<IPacketConsumer>>();
		_consumers[ptype].push_back(consumer);

	}
}
