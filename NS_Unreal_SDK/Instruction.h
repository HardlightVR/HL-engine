#pragma once
#include "StdAfx.h"
#include <vector>
#include "IJsonSerializable.h"
class Instruction : public IJsonSerializable
{
public:
	Instruction();
	~Instruction();
	std::vector<std::string> parameters;
	uint8_t ByteId;
	std::string Name;
	std::string Purpose;
	std::vector<std::string> Parameters;
	virtual void Serialize(Json::Value& root);
	virtual void Deserialize(Json::Value& root);
};

