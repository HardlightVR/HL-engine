#include "SuitHardwareInterface.h"
#include <fstream>

#include <iostream>

SuitHardwareInterface::SuitHardwareInterface()
{
	Json::Value root;
	std::ifstream instruction_json("Instructions.json", std::ifstream::binary);
	
	instruction_json >> root;

		this->builder.LoadInstructions(root);
	
	
}


SuitHardwareInterface::~SuitHardwareInterface()
{
}

void SuitHardwareInterface::SetAdapter(std::shared_ptr<ICommunicationAdapter> adapter) {
	this->adapter = adapter;
}

void SuitHardwareInterface::PlayEffect() {

	uint8_t packet[9] = { 0x24, 0x02, 0x13, 0x09, 0x15, 0x01, 0xFF, 0xFF, 0x0A };
	this->execute(packet, 9);
}

void SuitHardwareInterface::execute(uint8_t* packet, std::size_t length) {
	this->adapter->Write(packet, length);
}