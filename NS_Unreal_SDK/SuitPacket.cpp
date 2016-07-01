#include "SuitPacket.h"



SuitPacket::PacketType SuitPacket::Type()
{
	return this->packetType;
}

std::shared_ptr<const uint8_t> SuitPacket::Packet() const
{
	return std::shared_ptr<const uint8_t>(this->rawPacket);
}

SuitPacket::SuitPacket()
{
}


SuitPacket::~SuitPacket()
{
}
