#include "InstructionBuilder.h"



InstructionBuilder::InstructionBuilder()
{
	std::string validParams[4] = { "zone", "effect", "data", "register" };
	for (std::string param : validParams) {
		paramDict[param] = std::unordered_map<string, uint8_t>();
	}

}


InstructionBuilder::~InstructionBuilder()
{
}


InstructionBuilder InstructionBuilder::UseInstruction(std::string name) {
	this->parameters.clear();
	this->instruction = name;
	return *this;
}

InstructionBuilder InstructionBuilder::WithParam(std::string key, int val) {
	this->parameters[key] = EnumToString(val);
	return *this;
}

bool InstructionBuilder::Verify() {
	if (instructions.find(this->instruction) == instructions.end()) {
		return false;
	}

	Instruction desired = instructions[this->instruction];
	for (std::string param : desired.parameters) {
		if (parameters.find(param) == parameters.end()) {
			return false;
		}
		auto dict = this->paramDict[param];
		if (dict.find(parameters[param]) == dict.end()) {
			return false;
		}

	}
	return true;
}

Packet InstructionBuilder::Build() {
	Instruction desired = instructions[this->instruction];
	const int packetLength = 7 + this->parameters.size();
	uint8_t* packet = new uint8_t[packetLength];
	packet[0] = 0x24;
	packet[1] = 0x02;
	assert(desired.ByteId <= 255);
	packet[2] = desired.ByteId;
	assert(packetLength <= 255);
	packet[3] = packetLength;

	const std::size_t numParams = this->parameters.size();
	for (int i = 0; i < numParams; i++) {
		std::string paramKey = desired.parameters[i];
		std::string userParamVal = this->parameters[paramKey];
		auto paramKeyToByteId = this->paramDict[paramKey];
		uint8_t id = paramKeyToByteId[userParamVal];
		packet[i + 4] = id;
	}

	packet[packetLength - 3] = 0xFF;
	packet[packetLength - 2] = 0xFF;
	packet[packetLength - 1] = 0x0A;
	return Packet(packet, packetLength);
}

static std::string EnumToString(int s) {
	return "hi";
}