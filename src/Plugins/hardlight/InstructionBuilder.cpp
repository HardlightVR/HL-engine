#include "StdAfx.h"
#include "InstructionBuilder.h"
#include <iostream>
#include "Locator.h"



class param_valid_visitor : public boost::static_visitor<bool>
{
public:
	param_valid_visitor(const InstructionSet* iset, std::string key) :m_instructions(iset), m_key(key) {

	}
	bool operator()(int param) const {
		return true;
	}
	bool operator()(const std::string& param)const {
		const auto& dict = m_instructions->ParamDict();
		return dict.find(m_key) != dict.end();
	}
private:
	const InstructionSet* m_instructions;
	std::string m_key;
};

class param_value_visitor : public boost::static_visitor<uint8_t>
{
public:
	param_value_visitor(const InstructionSet* iset, std::string key) :m_instructions(iset), m_key(key) {
	}
	uint8_t operator()(int param) const {
		//warning: truncates to 0-255
		return uint8_t(param);
	}

	uint8_t operator()(const std::string& param) const {
		const auto& paramKeyToByteId = m_instructions->ParamDict().at(m_key);
		uint8_t result = paramKeyToByteId.at(param);
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



InstructionBuilder::InstructionBuilder(std::shared_ptr<InstructionSet> iset):_iset(iset)
{
	//todo: make iset a member if no one else needs it
	if (!_iset->LoadAll()) {
		Locator::Logger().Log("InstructionBuilder", "Couldn't load the instruction set! Will exit after you hit [any key]");
		std::cin.get();
		exit(0);
	}

}


InstructionBuilder::~InstructionBuilder()
{
}


InstructionBuilder& InstructionBuilder::UseInstruction(std::string name) {
	//std::cout << "Using instruction: " << name << '\n';
	this->_parameters.clear();
	this->_instruction = std::move(name);
	return *this;
}




InstructionBuilder& InstructionBuilder::WithParam(std::string key, Param val) {
	//std::cout << "	With param: " << key << " = " << val << '\n';
	this->_parameters[key] = val;
	return *this;
}



bool InstructionBuilder::Verify() const{
	
	const auto& instructions = _iset->Instructions();

	auto it = instructions.find(this->_instruction);

	if (it == instructions.end()) {
		return false;
	}

	const Instruction& desired = it->second;

	for (const std::string& param : desired.Parameters) {
		if (!boost::apply_visitor(param_valid_visitor(_iset.get(), param), _parameters.at(param))) {
			return false;
		}
	}
	return true;
}


std::string InstructionBuilder::GetDebugString() {
	std::string description = _instruction + ": ";
	std::size_t index = 0;
	if (_parameters.empty()) {
		description += "[no parameters]";
	}
	for (auto param : _parameters)
	{
		index++;
		description += param.first + " = " + boost::apply_visitor(param_debug_visitor(), param.second);
		if (index < _parameters.size())
		{
			description += ", ";
		}
	}
	return description;
}
std::vector<uint8_t> InstructionBuilder::Build() const {
	const Instruction& desired = _iset->Instructions().at(_instruction);
	const int packetLength = 7 + _parameters.size();
	
	std::vector<uint8_t> packet(packetLength, 0);

	packet[0] = 0x24;
	packet[1] = 0x02;
	assert(desired.ByteId <= 255);
	packet[2] = desired.ByteId;
	assert(packetLength <= 255);
	packet[3] = packetLength;

	const size_t numParams = _parameters.size();
	for (std::size_t i = 0; i < numParams; i++) {
		string paramKey = desired.Parameters[i];
		uint8_t val = boost::apply_visitor(param_value_visitor(_iset.get(), paramKey), _parameters.at(paramKey));
		packet[i + 4] = val;
	}

	packet[packetLength - 3] = 0xFF;
	packet[packetLength - 2] = 0xFF;
	packet[packetLength - 1] = 0x0A;
	
	return packet;
}



