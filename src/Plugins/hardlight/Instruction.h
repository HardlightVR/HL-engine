#pragma once
#include <stdint.h>
#include "json/json-forwards.h"
#include <vector>

class Instruction 
{
public:
	Instruction();
	~Instruction();
	uint8_t ByteId;
	std::string Name;
	std::string Purpose;
	std::vector<std::string> Parameters;
	virtual void Serialize(const Json::Value& root);
	virtual void Deserialize(const Json::Value& root);
};

