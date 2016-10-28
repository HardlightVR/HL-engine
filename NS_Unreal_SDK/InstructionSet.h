#pragma once
#include <unordered_map>
#include "Instruction.h"
#include "json\json.h"

class InstructionSet
{
public:
	InstructionSet();
	~InstructionSet(); 
	bool LoadAll();
	bool LoadInstructions(const Json::Value& json);
	bool LoadZones(const Json::Value& json);
	bool LoadEffects(const Json::Value& json);
	const std::unordered_map<std::string, Instruction>& Instructions() const;
	const std::unordered_map < std::string, std::unordered_map<std::string, uint8_t>>&  ParamDict() const;
private:
	std::unordered_map<std::string, Instruction> _instructions;
	std::unordered_map<std::string, std::unordered_map<std::string, uint8_t>> _paramDict;
	bool LoadKeyValue(std::unordered_map<std::string, uint8_t>& dict, Json::Value json);

};

