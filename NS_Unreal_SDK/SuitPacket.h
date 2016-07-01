#pragma once
#include "StdAfx.h"
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
	static constexpr int SUIT_PACKET_LENGTH = 16;
	PacketType Type();
	std::shared_ptr<const uint8_t> Packet() const;
	SuitPacket();
	~SuitPacket();
private:
	uint8_t rawPacket[SUIT_PACKET_LENGTH];
	PacketType packetType;
};

