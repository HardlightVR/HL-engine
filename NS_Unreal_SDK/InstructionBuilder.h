#pragma once
#include <unordered_map>
#include "Instruction.h"
#include "Packet.h"
#include "json\json.h"
using std::string;
class InstructionBuilder
{
public:
	InstructionBuilder();
	~InstructionBuilder();
	InstructionBuilder& UseInstruction(string name);
	InstructionBuilder& WithParam(std::string key, std::string val);
	bool Verify();
	Packet Build();
	bool LoadInstructions(const Json::Value& json);
	bool LoadZones(const Json::Value& json);
	bool LoadEffects(const Json::Value& json);
	std::string GetDebugString();

private:
	std::unordered_map<string, Instruction> _instructions;
	std::string _instruction;

	std::unordered_map<string, string> _parameters;
	std::unordered_map<string, std::unordered_map<string, uint8_t>> _paramDict;
	bool LoadKeyValue(std::unordered_map<string, uint8_t>& dict, Json::Value json);
};

