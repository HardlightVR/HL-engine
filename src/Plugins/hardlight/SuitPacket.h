#pragma once
#include "suit_packet.h"
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
		DummyTracking = 0x99,
		Undefined
	};
	static SuitPacket::PacketType Type(const packet& p) {
		return SuitPacket::PacketType(p.raw[2]);
	}
	SuitPacket::PacketType Type() const;
	packet Packet() const;
	SuitPacket() = delete;
	~SuitPacket() = delete;

};

