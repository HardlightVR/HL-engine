#include "Packet.h"



Packet::Packet(uint8_t* raw, std::size_t length)
{
	this->Data = raw;
	this->Length = length;
}


Packet::~Packet()
{
	free(this->Data);
}
