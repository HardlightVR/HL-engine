#pragma once
#include "StdAfx.h"
#include <vector>
class Instruction
{
public:
	Instruction();
	~Instruction();
	std::vector<string> parameters;
	uint8_t ByteId;
private:
	uint8_t id;
};

