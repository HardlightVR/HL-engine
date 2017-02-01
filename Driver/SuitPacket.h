#pragma once
#include "Synchronizer.h"
class SuitPacket
{
public:
	enum class PacketType {
		SuitVersion = 0x01,
		Ping = 0x02,
		SuitStatus = 0x03,
		DrvStatus = 0x15,
		ImuData = 0x33,
		FifoOverflow = 0x34,
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

