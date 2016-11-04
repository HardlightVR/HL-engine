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
