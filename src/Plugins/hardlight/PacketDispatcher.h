#pragma once
#include "SuitPacket.h"
#include <unordered_map>
#include <functional>

#include <boost/signals2.hpp>
class PacketDispatcher
{
public:
	using PacketEvent = boost::signals2::signal<void(Packet)>;
	PacketDispatcher();
	~PacketDispatcher() { std::cout << "DISPATCHER DESTRUCTOR\n"; }
	void Dispatch(Packet packet);
	void ClearConsumers();
	void AddConsumer(PacketType ptype, PacketEvent::slot_type packetFunc);
private:
	std::unordered_map<PacketType, std::unique_ptr<PacketEvent>> m_consumers;
};

