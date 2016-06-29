#include "Instruction.h"
#include "json\json.h"
#include <string>
#include "HexUtils.h"
Instruction::Instruction()
{
}


Instruction::~Instruction()
{
}



void Instruction::Serialize(Json::Value& root) {

}

void Instruction::Deserialize(Json::Value& root) {
	std::string byteId = root.get("id", "00").asString();
	uint8_t result[1];
	HexStringToInt(&byteId.c_str()[2], result);
	this->ByteId = result[0];
	this->Name = root.get("name", "Unnamed Instruction").asString();
	this->Purpose = root.get("purpose", "Unspecified Purpose").asString();
	for (const Json::Value& book : root["parameters"]) {
		this->parameters.push_back(book.asString());
	}
}


