#include "stdafx.h"
#include "PacketDispatcher.h"
#include "logger.h"



PacketDispatcher::PacketDispatcher() : m_consumers()
{
}
void PacketDispatcher::Dispatch(Packet Packet)
{
	auto packetType = GetType(Packet);
	
	auto it = m_consumers.find(packetType);

	if (it != m_consumers.end())
	{
		it->second->operator()(Packet);
	}
	else {
		if (inst::Id::_is_valid(packetType)) {
			core_log(nsvr_severity_info, "Dispatcher", "No consumers for packet type " + std::string(packetType._to_string()));
		}
		else {
			core_log(nsvr_severity_info, "Dispatcher", "No consumers for packet type " + std::to_string(packetType));

		}
	}
}

void PacketDispatcher::ClearConsumers()
{
	m_consumers.clear();
}

void PacketDispatcher::AddConsumer(inst::Id ptype, PacketDispatcher::PacketEvent::slot_type packetFunc)
{
	auto it = m_consumers.find(ptype);
	if (it != m_consumers.end()) {
		it->second->connect(packetFunc);
	}
	else {
		m_consumers.insert(std::make_pair(ptype, std::make_unique<PacketEvent>()));
		m_consumers.at(ptype)->connect(packetFunc);

	}
}

