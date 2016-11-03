#include "InstructionSet.h"

#include <fstream>
#include "HexUtils.h"


InstructionSet::InstructionSet():_instructions()
{
	std::string validParams[4] = { "zone", "effect", "data", "register" };
	for (std::string param : validParams) {
		_paramDict[param] = std::unordered_map<std::string, uint8_t>();
	}
}


InstructionSet::~InstructionSet()
{
}

bool InstructionSet::LoadEffects(const Json::Value& json) {
	return LoadKeyValue(this->_paramDict["effect"], json);
}

const std::unordered_map<std::string, Instruction>& InstructionSet::Instructions() const
{
	return _instructions;
}

const std::unordered_map<std::string, std::unordered_map<std::string, uint8_t>>& InstructionSet::ParamDict() const
{
	return _paramDict;
}

bool InstructionSet::LoadZones(const Json::Value& json) {
	return LoadKeyValue(this->_paramDict["zone"], json);
}


bool InstructionSet::LoadAll() {
	//Todo: Better error checking here, with actual exceptions/error codes

	Json::Value inst_root;
	std::ifstream instruction_json("Instructions.json", std::ifstream::binary);
	instruction_json >> inst_root;
	bool gotInstructions = LoadInstructions(inst_root);

	Json::Value zone_root;
	std::ifstream zone_json("Zones.json", std::ifstream::binary);
	zone_json >> zone_root;
	bool gotZones = LoadZones(zone_root);

	Json::Value effect_root;
	std::ifstream effect_json("Effects.json", std::ifstream::binary);
	effect_json >> effect_root;
	bool gotEffects = LoadEffects(effect_root);

	return gotZones && gotInstructions && gotEffects;
}
bool InstructionSet::LoadInstructions(const Json::Value& json) {
	std::size_t numInstructions = json.size();
	for (int i = 0; i < numInstructions; ++i) {
		Instruction inst;
		inst.Deserialize(json[i]);
		_instructions[inst.Name] = inst;
	}
	return true;
}


bool InstructionSet::LoadKeyValue(std::unordered_map<std::string, uint8_t>& dict, Json::Value json) {
	auto names = json.getMemberNames();


	for (std::string key : names) {
		std::string val = json.get(key, "0x00").asString();
		const char* hexChars = &val.c_str()[2];
		uint8_t hex[1]{ 0 };
		HexStringToInt(hexChars, hex);
		dict[key] = hex[0];
	}



	return true;
}