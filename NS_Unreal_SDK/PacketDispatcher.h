#pragma once
#include "Synchronizer.h"
#include <unordered_map>
#include "SuitPacket.h"
#include "IPacketConsumer.h"
using namespace std;

class PacketDispatcher
{
public:
	PacketDispatcher();
	~PacketDispatcher();
	void Dispatch(packet packet);
	void AddConsumer(SuitPacket::PacketType ptype, std::shared_ptr<IPacketConsumer> consumer);
private:
	unsigned int _dispatchLimit;
	unordered_map<SuitPacket::PacketType, std::vector<std::shared_ptr<IPacketConsumer>>> _consumers;
};

