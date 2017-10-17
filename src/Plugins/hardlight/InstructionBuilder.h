#pragma once
#include <unordered_map>
#include "Instruction.h"
#include "Packet.h"
#include "InstructionSet.h"
#include <boost/variant.hpp>
using std::string;
class InstructionBuilder
{
	typedef boost::variant<std::string, int> Param;
public:
	explicit InstructionBuilder(std::shared_ptr<InstructionSet> iset);
	~InstructionBuilder();
	InstructionBuilder& UseInstruction(string name);
	InstructionBuilder& WithParam(std::string key, Param val);
	bool Verify();
	std::vector<uint8_t> Build() const;
	std::string GetDebugString();

private:
	std::string _instruction;
	std::shared_ptr<InstructionSet> _iset;
	std::unordered_map<string, Param> _parameters;
};

class param_valid_visitor : public boost::static_visitor<bool>
{
public:
	param_valid_visitor(std::shared_ptr<InstructionSet>& iset, std::string key):m_instructions(iset), m_key(key) {

	}
	bool operator()(int param) const {
		return true;
	}
	bool operator()(const std::string& param)const {
		const auto& dict = m_instructions->ParamDict();
		return dict.find(m_key) != dict.end();
	}
private:
	std::shared_ptr<InstructionSet>& m_instructions;
	std::string m_key;
};

class param_value_visitor : public boost::static_visitor<uint8_t>
{
public :
	param_value_visitor(const InstructionSet* iset, std::string key):m_instructions(iset), m_key(key) {
	}
	uint8_t operator()(int param) const {
		//warning: truncates to 0-255
		return uint8_t(param);
	}

	uint8_t operator()(const std::string& param) const {
		const auto& paramKeyToByteId = m_instructions->ParamDict().at(m_key);
		uint8_t result =  paramKeyToByteId.at(param);
		return result;
	}


private:
	const InstructionSet* m_instructions;
	std::string m_key;
};


class param_debug_visitor : public boost::static_visitor<std::string>
{
public:
	param_debug_visitor() {}
	
	std::string operator()(int param) const {
		//warning: truncates to 0-255
		return std::to_string(param);
	}

	std::string operator()(const std::string& param) const {
		return param;
	}


};
