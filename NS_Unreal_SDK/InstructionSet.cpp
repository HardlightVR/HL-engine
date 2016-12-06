#include "StdAfx.h"
#include "InstructionSet.h"

#include <fstream>


InstructionSet::InstructionSet() :_instructions()
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


void InstructionSet::loadFromJson(std::string path, Loader loader) {
	Json::Value root;
	std::ifstream json(path, std::ifstream::binary);
	try {
		json >> root;
		loader(root); 
	}
	catch (Json::Exception::exception& e) {
		throw InstructionLoadingException("Failed to load " + path + ": " + e.what());
	}
}
bool InstructionSet::LoadAll() {
	try {
		
		loadFromJson("C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/NS_Unreal_SDK/Zones.json",
			boost::bind(&InstructionSet::LoadKeyValue, this, boost::ref(_paramDict["zone"]), _1));
		
		loadFromJson("C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/NS_Unreal_SDK/Effects.json",
			boost::bind(&InstructionSet::LoadKeyValue, this, boost::ref(_paramDict["effect"]), _1));

		loadFromJson("C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/NS_Unreal_SDK/Instructions.json", 
			boost::bind(&InstructionSet::LoadInstructions, this, _1));

		loadFromJson("C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/NS_Unreal_SDK/Atoms.json",
			boost::bind(&InstructionSet::LoadAtoms, this, _1));

	


		return true;

	}
	catch (InstructionLoadingException& e) {
		std::cout << e.what() << '\n';
		return false;
	}
}
void InstructionSet::LoadInstructions(const Json::Value& json) {
	
		std::size_t numInstructions = json.size();
		for (int i = 0; i < numInstructions; ++i) {
			Instruction inst;
			inst.Deserialize(json[i]);
			_instructions[inst.Name] = inst;
		}

	
}

void InstructionSet::LoadAtoms(const Json::Value & json)
{
	std::size_t numAtoms = json.size();
	for (int i = 0; i < numAtoms; ++i) {
		Atom atom;
		atom.Deserialize(json[i]);
		_atoms[atom.Id()] = atom;
	}
}


void InstructionSet::LoadKeyValue(std::unordered_map<std::string, uint8_t>& dict, Json::Value json) {
	
	auto names = json.getMemberNames();
	for (std::string key : names) {
		std::string val = json.get(key, "0x00").asString();
		const char* hexChars = &val.c_str()[2];
		uint8_t hex[1]{ 0 };
		HexStringToInt(hexChars, hex);
		dict[key] = hex[0];
	}
	
	
}