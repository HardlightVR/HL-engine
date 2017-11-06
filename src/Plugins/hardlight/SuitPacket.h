#pragma once
#include "suit_packet.h"
#include "Instructions.h"

inline inst::Id GetType(const Packet& packet) {
	return static_cast<inst::Id::_enumerated>(packet[2]);
}

