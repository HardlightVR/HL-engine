#pragma once
#include "SuitPacket.h"
#include <unordered_map>
#include <functional>

#include <boost/signals2.hpp>
#include "Instructions.h"
class PacketDispatcher
{
public:
	using PacketEvent = boost::signals2::signal<void(Packet)>;
	~PacketDispatcher() { std::cout << "DESTROYING PACKETDISPATCHER\n"; }
	void Dispatch(Packet packet);
	void ClearConsumers();
	void AddConsumer(inst::Id, PacketEvent::slot_type packetFunc);
private:
	std::unordered_map<inst::Id::_enumerated, std::unique_ptr<PacketEvent>> m_consumers;
};

