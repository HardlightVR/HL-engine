#pragma once
#include "suit_packet.h"
struct SuitVersionInfo {
	unsigned int Major;
	unsigned int Minor;
	SuitVersionInfo(const packet& packet) :Major(packet.raw[3]), Minor(packet.raw[4]) {}
};