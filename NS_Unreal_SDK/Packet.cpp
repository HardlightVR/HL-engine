#include "StdAfx.h"
#include "Packet.h"



Packet::Packet(uint8_t* raw, std::size_t length)
{
	//assert(raw != nullptr);
	this->Data = new uint8_t[length];
	memcpy(this->Data, raw, length);
	this->Length = length;
}


Packet::~Packet()
{
	//assert(Data != nullptr);
		delete[] Data;
		Data = nullptr;
}

Packet::Packet(const Packet& b)
{
	Length = b.Length;
	Data = new uint8_t[b.Length];
	memcpy(this->Data, b.Data, b.Length);
	

}

Packet & Packet::operator=(const Packet & other)
{
	if (this != &other) {
		uint8_t* new_data = new uint8_t[other.Length];
		std::copy(other.Data, other.Data + other.Length, new_data);

		//delete our old packet data
		delete[] Data;

		//assign the pointer to the new'd up array, and copy over the length
		Data = new_data;
		Length = other.Length;
		
	}
	return *this;
}
