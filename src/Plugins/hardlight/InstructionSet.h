#pragma once
#include <unordered_map>
#include "Instruction.h"
#include "Atom.h"
#include "Json/json-forwards.h"
#include <functional>
typedef std::function<void(const Json::Value& json)> Loader;


class InstructionLoadingException : public std::exception
{
public:
	InstructionLoadingException(std::string m):_message(m) {

	}

	virtual const char* what() const throw()
	{
		return _message.c_str();
	}
private:
	std::string _message;
};

class InstructionSet
{
public:
	InstructionSet(const std::string& data_dir);
	~InstructionSet(); 
	bool LoadAll();
	const std::unordered_map<std::string, Instruction>& Instructions() const;
	const std::unordered_map<std::string, std::unordered_map<std::string, uint8_t>>&  ParamDict() const;
	const std::unordered_map<std::string, Atom>& Atoms() const;

private:
	std::string _dataDir;
	std::unordered_map<std::string, Atom> _atoms;

	std::unordered_map<std::string, Instruction> _instructions;
	std::unordered_map<std::string, std::unordered_map<std::string, uint8_t>> _paramDict;



};

