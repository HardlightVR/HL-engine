#pragma once

#include "Instructions.h"


#include <cstdint>
#include <array>
const unsigned int PACKET_LENGTH = 16;

using Packet = std::array<uint8_t, PACKET_LENGTH>;

inline inst::Id GetType(const Packet& packet) {
	return static_cast<inst::Id::_enumerated>(packet[2]);
}
