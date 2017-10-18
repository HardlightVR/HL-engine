#include "stdafx.h"
#include "PacketDispatcher.h"


PacketDispatcher::PacketDispatcher() : m_consumers()
{
}
void PacketDispatcher::Dispatch(Packet Packet)
{
	PacketType packetType = GetType(Packet);
	if (m_consumers.find(packetType) != m_consumers.end())
	{
		for (const auto& consumer : m_consumers.at(packetType))
		{
			consumer(Packet);
		}
	}
}

void PacketDispatcher::AddConsumer(PacketType ptype, OnReceivePacketFunc packetFunc)
{
	m_consumers[ptype].push_back(packetFunc);
}

