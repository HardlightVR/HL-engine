#pragma once
#include "json\json-forwards.h"
class IJsonSerializable
{
public:
	virtual ~IJsonSerializable(void) {};
	virtual void Serialize(const Json::Value& root) = 0;
	virtual void Deserialize(const Json::Value& root) = 0;
};