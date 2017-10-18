#pragma once
#include <cstdint>
#include <array>
const unsigned int PACKET_LENGTH = 16;

using packet = std::array<uint8_t, PACKET_LENGTH>;

//struct packet
//{
//	uint8_t raw[PACKET_LENGTH];
//	unsigned int size = PACKET_LENGTH;
//};