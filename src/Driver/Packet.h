#pragma once
#include <iostream>
class Packet
{
public:
	Packet(uint8_t* raw, std::size_t length);
	
	~Packet();
	std::size_t Length;
	uint8_t* Data;
	Packet(const Packet& b);
	Packet& operator=(const Packet& other);
};

