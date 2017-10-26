#pragma once
#include <cstdint>
#include <array>
const unsigned int PACKET_LENGTH = 16;

using Packet = std::array<uint8_t, PACKET_LENGTH>;
