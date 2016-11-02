#include "InstructionBuilder.h"
#include <iostream>

InstructionBuilder::InstructionBuilder(std::shared_ptr<InstructionSet> iset):_iset(iset)
{
	

}


InstructionBuilder::~InstructionBuilder()
{
}


InstructionBuilder& InstructionBuilder::UseInstruction(std::string name) {
	std::cout << "Using instruction: " << name << '\n';
	this->_parameters.clear();
	this->_instruction = std::move(name);
	return *this;
}




InstructionBuilder& InstructionBuilder::WithParam(std::string key, std::string val) {
	//std::cout << "	With param: " << key << " = " << val << '\n';
	this->_parameters[key] = val;
	return *this;
}

bool InstructionBuilder::Verify() {
	

	if (_iset->Instructions().find(this->_instruction) == _iset->Instructions().end()) {
	//	std::cout << "		Couldn't find " << this->_instruction << '\n';
		return false;
	}

	const Instruction& desired = _iset->Instructions().at(this->_instruction);
	for (std::string param : desired.Parameters) {
		if (_parameters.find(param) == _parameters.end()) {
		//	std::cout << "		Couldn't find " << param << '\n';

			return false;
		}
		auto dict = this->_iset->ParamDict().at(param);
		if (dict.find(_parameters[param]) == dict.end()) {
			//std::cout << "		Couldn't find the dict " << param << '\n';

			return false;
		}

	}
//	std::cout << "	verified." << '\n';
	return true;
}


std::string InstructionBuilder::GetDebugString() {
	std::string description = _instruction + ": ";
	std::size_t index = 0;
	for (auto param : _parameters)
	{
		index++;
		description += param.first + " = " + param.second;
		if (index < _parameters.size())
		{
			description += ", ";
		}
	}
	return description;
}
Packet InstructionBuilder::Build() {
	const Instruction& desired = _iset->Instructions().at(_instruction);
	const int packetLength = 7 + _parameters.size();
	uint8_t* packet = new uint8_t[packetLength];
	
	std::fill(packet, packet + packetLength, 0);
	packet[0] = 0x24;
	packet[1] = 0x02;
	//assert(desired.ByteId <= 255);
	packet[2] = desired.ByteId;
	//assert(packetLength <= 255);
	packet[3] = packetLength;

	const size_t numParams = _parameters.size();
	for (std::size_t i = 0; i < numParams; i++) {
		string paramKey = desired.Parameters[i];
		string userParamVal = _parameters[paramKey];
		auto paramKeyToByteId =_iset->ParamDict().at(paramKey);
		uint8_t id = paramKeyToByteId[userParamVal];
		packet[i + 4] = id;
	}

	packet[packetLength - 3] = 0xFF;
	packet[packetLength - 2] = 0xFF;
	packet[packetLength - 1] = 0x0A;
	Packet retPacket(packet, packetLength);
	delete[] packet;
	return retPacket;
}



