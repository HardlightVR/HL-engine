#pragma once
#include "SuitPacket.h"
#include <unordered_map>
#include <functional>


class PacketDispatcher
{
public:
	using OnReceivePacketFunc = std::function<void(Packet)>;
	PacketDispatcher();
	void Dispatch(Packet packet);
	void AddConsumer(PacketType ptype, OnReceivePacketFunc packetFunc);
private:
	std::unordered_map<PacketType, std::vector<OnReceivePacketFunc>> m_consumers;
};

