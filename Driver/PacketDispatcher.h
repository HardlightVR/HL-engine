#pragma once
#include "SuitPacket.h"
#include "IPacketConsumer.h"
#include <unordered_map>
using namespace std;

class PacketDispatcher
{
public:
	typedef std::function<void(packet)> OnReceivePacketFunc;
	PacketDispatcher();
	~PacketDispatcher();
	void Dispatch(packet packet);
	void AddConsumer(SuitPacket::PacketType ptype, OnReceivePacketFunc packetFunc);
private:
	unsigned int _dispatchLimit;
	std::unordered_map<SuitPacket::PacketType, std::vector<OnReceivePacketFunc>> _consumers;
};

