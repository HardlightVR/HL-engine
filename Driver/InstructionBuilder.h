#pragma once
#include <unordered_map>
#include "Instruction.h"
#include "Packet.h"
#include "InstructionSet.h"
using std::string;
class InstructionBuilder
{
public:
	InstructionBuilder(std::shared_ptr<InstructionSet> iset);
	~InstructionBuilder();
	InstructionBuilder& UseInstruction(string name);
	InstructionBuilder& WithParam(std::string key, std::string val);
	bool Verify();
	Packet Build();
	std::string GetDebugString();

private:
	std::string _instruction;
	std::shared_ptr<InstructionSet> _iset;
	std::unordered_map<string, string> _parameters;
};

