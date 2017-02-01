#include "StdAfx.h"
#include "SuitPacket.h"



SuitPacket::PacketType SuitPacket::Type() const
{
	return _packetType;
}

packet SuitPacket::Packet() const
{
	return _rawPacket;
}

SuitPacket::SuitPacket(packet p):_rawPacket(p)
{
	//todo: this will allow random values to slip through,
	//but may not be a problem if we disregard them
	_packetType = PacketType(_rawPacket.raw[2]);
}


SuitPacket::~SuitPacket()
{
}
