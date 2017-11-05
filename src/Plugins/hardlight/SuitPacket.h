#pragma once
#include "suit_packet.h"
#include "Instructions.h"
enum class PacketType {
	SuitVersion = 0x01,
	Ping = 0x02,
	Uuid = 0x03,
	DrvStatus = 0x15,
	ImuData = 0x33,
	FifoOverflow = 0x34,
	DummyTracking = 0x99,
	ImuStatus = inst::Id::GET_TRACK_STATUS,
	Undefined
};

inline PacketType GetType(const Packet& packet) {
	return static_cast<PacketType>(packet[2]);
}

