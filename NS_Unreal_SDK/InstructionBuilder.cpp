#include "InstructionBuilder.h"
#include <iostream>
#include "HexUtils.h"
#include <fstream>
InstructionBuilder::InstructionBuilder()
{
	std::string validParams[4] = { "zone", "effect", "data", "register" };
	for (std::string param : validParams) {
		_paramDict[param] = std::unordered_map<string, uint8_t>();
	}

}
static std::string EnumToString(int s) {
	return "hi";
}


InstructionBuilder::~InstructionBuilder()
{
}


InstructionBuilder& InstructionBuilder::UseInstruction(std::string name) {
	this->_parameters.clear();
	this->_instruction = name;
	return *this;
}




InstructionBuilder& InstructionBuilder::WithParam(std::string key, std::string val) {
	this->_parameters[key] = val;
	return *this;
}

bool InstructionBuilder::Verify() {
	if (_instructions.find(this->_instruction) == _instructions.end()) {
		return false;
	}

	Instruction desired = _instructions[this->_instruction];
	for (std::string param : desired.Parameters) {
		if (_parameters.find(param) == _parameters.end()) {
			return false;
		}
		auto dict = this->_paramDict[param];
		if (dict.find(_parameters[param]) == dict.end()) {
			return false;
		}

	}
	return true;
}


std::string InstructionBuilder::GetDebugString() {
	std::string description = this->_instruction + ": ";
	std::size_t index = 0;
	for (auto param : this->_parameters)
	{
		index++;
		description += param.first + " = " + param.second;
		if (index < this->_parameters.size())
		{
			description += ", ";
		}
	}
	return description;
}
Packet InstructionBuilder::Build() {
	Instruction desired = _instructions[this->_instruction];
	const int packetLength = 7 + this->_parameters.size();
	uint8_t* packet = new uint8_t[packetLength];
	
	std::fill(packet, packet + packetLength, 0);
	packet[0] = 0x24;
	packet[1] = 0x02;
	assert(desired.ByteId <= 255);
	packet[2] = desired.ByteId;
	assert(packetLength <= 255);
	packet[3] = packetLength;

	const size_t numParams = _parameters.size();
	for (std::size_t i = 0; i < numParams; i++) {
		string paramKey = desired.Parameters[i];
		string userParamVal = _parameters[paramKey];
		auto paramKeyToByteId = _paramDict[paramKey];
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


bool InstructionBuilder::LoadKeyValue(std::unordered_map<string, uint8_t>& dict, Json::Value json) {
	auto names = json.getMemberNames();


	for (std::string key : names) {
		std::string val = json.get(key, "0x00").asString();
		const char* hexChars = &val.c_str()[2];
		uint8_t hex[1]{ 0 };
		HexStringToInt(hexChars, hex);
		dict[key] = hex[0];
	}
	

	
	return false;
}

bool InstructionBuilder::LoadEffects(const Json::Value& json) {
	return LoadKeyValue(this->_paramDict["effect"], json);
}

bool InstructionBuilder::LoadZones(const Json::Value& json) {
	return LoadKeyValue(this->_paramDict["zone"], json);
}

bool InstructionBuilder::LoadInstructions(const Json::Value& json) {
	std::size_t numInstructions = json.size();
	for (std::size_t i = 0; i < numInstructions; ++i) {
		Instruction inst;
		inst.Deserialize(json[i]);
		_instructions[inst.Name] = inst;
	}
	return false;
}

