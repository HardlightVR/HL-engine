#pragma once
#include "IJsonSerializable.h"

class Instruction : public IJsonSerializable
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

