#include "InstructionBuilder.h"
#include <iostream>
#include "HexUtils.h"

InstructionBuilder::InstructionBuilder()
{
	std::string validParams[4] = { "zone", "effect", "data", "register" };
	for (std::string param : validParams) {
		paramDict[param] = std::unordered_map<string, uint8_t>();
	}

}
static std::string EnumToString(int s) {
	return "hi";
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
	for (std::size_t i = 0; i < numParams; i++) {
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


bool InstructionBuilder::LoadKeyValue(std::unordered_map<string, uint8_t> dict, Json::Value json) {
	std::size_t numKeys = json.size();
	for (std::size_t i = 0; i < numKeys; ++i) {
		uint8_t value[1];
		HexStringToInt(&json[i].asString().c_str()[2], value);
		Json::Value whatever = json[i];
		int x = 13;
	}
	return false;
}

bool InstructionBuilder::LoadInstructions(Json::Value json) {
	std::size_t numInstructions = json.size();
	for (std::size_t i = 0; i < numInstructions; ++i) {
		Instruction inst;
		inst.Deserialize(json[i]);
		instructions[inst.Name] = inst;
	}
	return false;
}

