#pragma once
#include "suit_packet.h"
#include <unordered_map>
#include <functional>

#include <boost/signals2.hpp>
#include "Instructions.h"
class PacketDispatcher
{
public:
	using PacketEvent = boost::signals2::signal<void(Packet)>;
	void Dispatch(Packet packet);
	void ClearConsumers();
	void AddConsumer(inst::Id, PacketEvent::slot_type packetFunc);
private:
	std::unordered_map<inst::Id::_enumerated, std::unique_ptr<PacketEvent>> m_consumers;
};

