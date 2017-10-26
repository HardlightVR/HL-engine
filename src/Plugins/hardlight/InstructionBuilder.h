#pragma once
#include <unordered_map>
#include "Instruction.h"
#include "InstructionSet.h"
#include <boost/variant.hpp>


using std::string;
class InstructionBuilder
{
	using Param = boost::variant<std::string, int>;
public:
	explicit InstructionBuilder(std::shared_ptr<InstructionSet> iset);
	~InstructionBuilder();
	InstructionBuilder& UseInstruction(string name);
	InstructionBuilder& WithParam(std::string key, Param val);
	bool Verify() const;
	std::vector<uint8_t> Build() const;
	std::string GetDebugString();

private:
	std::string _instruction;
	std::shared_ptr<InstructionSet> _iset;
	std::unordered_map<string, Param> _parameters;
};
