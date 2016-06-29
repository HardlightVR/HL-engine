#pragma once
#include "StdAfx.h"
class Packet
{
public:
	Packet(uint8_t* raw, std::size_t length);
	~Packet();
	std::size_t Length;
	uint8_t* Data;
};

