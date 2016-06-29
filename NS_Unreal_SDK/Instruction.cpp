#include "Instruction.h"
#include "json\json.h"
#include <string>

Instruction::Instruction()
{
}


Instruction::~Instruction()
{
}

int char2int(char input)
{
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	if (input >= 'a' && input <= 'f')
		return input - 'a' + 10;
	throw std::invalid_argument("Invalid input string");
}


void hex2bin(const char* src, uint8_t* target)
{
	while (*src && src[1])
	{
		*(target++) = char2int(*src) * 16 + char2int(src[1]);
		src += 2;
	}
}

void Instruction::Serialize(Json::Value& root) {

}

void Instruction::Deserialize(Json::Value& root) {
	std::string byteId = root.get("id", "00").asString();
	uint8_t result[1];
	hex2bin(&byteId.c_str()[2], result);
	this->ByteId = result[0];
	this->Name = root.get("name", "Unnamed Instruction").asString();
	this->Purpose = root.get("purpose", "Unspecified Purpose").asString();
	for (const Json::Value& book : root["parameters"]) {
		this->parameters.push_back(book.asString());
	}
}


