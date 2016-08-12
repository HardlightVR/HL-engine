#pragma once
#include "StdAfx.h"
#include "Synchronizer.h"
class SuitPacket
{
public:
	enum class PacketType {
		ImuData = 0x33,
		ImuStatus,
		DrvStatus = 0x15,
		Ping = 0x02,
		Undefined
	};
	PacketType Type() const;
	packet Packet() const;
	SuitPacket(packet p);
	~SuitPacket();
private:
	packet _rawPacket;
	PacketType _packetType;
};

