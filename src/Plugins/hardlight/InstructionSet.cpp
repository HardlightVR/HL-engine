#include "StdAfx.h"
#include "InstructionSet.h"
#include "HexUtils.h"
#include <fstream>
#include "Json/json.h"

#include "JsonKeyValueConfig.h"

InstructionSet::InstructionSet(const std::string& data_dir) :_instructions(), _dataDir(data_dir)
{
	std::string validParams[4] = { "zone", "effect", "data", "register" };
	for (std::string param : validParams) {
		_paramDict[param] = std::unordered_map<std::string, uint8_t>();
	}
}


InstructionSet::~InstructionSet()
{
}


const std::unordered_map<std::string, Instruction>& InstructionSet::Instructions() const
{
	return _instructions;
}

const std::unordered_map<std::string, std::unordered_map<std::string, uint8_t>>& InstructionSet::ParamDict() const
{
	return _paramDict;
}



const std::unordered_map<std::string, Atom>& InstructionSet::Atoms() const
{
	return _atoms;
}


bool InstructionSet::LoadAll() {
	try {
		using namespace nsvr::tools::json;

		//Extract an actual byte from a hex string
		auto parseHexValue = [](const Json::Value& val) {
			const char* hexChars = &val.asCString()[2];
			uint8_t hex[1]{ 0 };
			HexStringToInt(hexChars, hex);
			return hex[0];
		};


		//Zones & Effects are dictionaries, so we grab the key and parse the hex value
		_paramDict["zone"] = parseDictFromDict<std::string, uint8_t>(_dataDir + "/Zones.json", [](auto val) { return val.asString(); }, parseHexValue);

		_paramDict["effect"] = parseDictFromDict<std::string, uint8_t>(_dataDir + "/Effects.json", [](auto val) { return val.asString(); }, parseHexValue);
		
	
	
		//Instructions and atoms are more complex objects, so we deserialize them manually
		_instructions = parseDictFromArray<std::string, Instruction>(_dataDir + "/Instructions.json",
		[](auto value) {
			Instruction inst;
			inst.Deserialize(value);
			return std::make_tuple(inst.Name, inst);
		});

		_atoms = parseDictFromArray<std::string, Atom>(_dataDir + "/Atoms.json",
		[](auto value) {
			Atom atom;
			atom.Deserialize(value);
			return std::make_tuple(atom.Id(), atom);
		});
	
		return true;
	}
	catch (Json::Exception& e) {
		std::cout << e.what() << '\n';
		return false;
	}
}

