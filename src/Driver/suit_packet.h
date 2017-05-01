#pragma once
#include <stdint.h>

const unsigned int PACKET_LENGTH = 16;

struct packet
{
	uint8_t raw[PACKET_LENGTH];
	unsigned int size = PACKET_LENGTH;
};