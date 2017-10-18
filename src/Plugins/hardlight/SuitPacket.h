#pragma once
#include "suit_packet.h"

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

inline PacketType GetType(const Packet& packet) {
	return static_cast<PacketType>(packet[2]);
}

