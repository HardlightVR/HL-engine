#include "stdafx.h"
#include "PacketDispatcher.h"
#include "logger.h"

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
	else {
		core_log(nsvr_severity_info, "Dispatcher", "Packet type wasn't found: " + std::to_string((int)packetType));
	}
}

void PacketDispatcher::AddConsumer(PacketType ptype, OnReceivePacketFunc packetFunc)
{
	m_consumers[ptype].push_back(packetFunc);
}

