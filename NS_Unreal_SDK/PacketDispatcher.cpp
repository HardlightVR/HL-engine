#include "PacketDispatcher.h"


PacketDispatcher::PacketDispatcher(std::shared_ptr<CircularBuffer> data) :_synchronizer(data, *this), _dispatchLimit(32)
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
