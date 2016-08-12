#pragma once
#include "Synchronizer.h"
#include <unordered_map>
#include "SuitPacket.h"
#include "IPacketConsumer.h"
using namespace std;

class PacketDispatcher
{
public:
	PacketDispatcher(std::shared_ptr<CircularBuffer> data);
	~PacketDispatcher();
	void Dispatch(packet packet);
private:
	Synchronizer _synchronizer;
	unsigned int _dispatchLimit;
	unordered_map<SuitPacket::PacketType, std::vector<std::shared_ptr<IPacketConsumer>>> _consumers;
};

