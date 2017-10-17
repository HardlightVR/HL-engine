#include "stdafx.h"
#include "Instructions.h"
#include <cassert>
#include "InstructionSet.h"
#include <sstream>
const static int packet_header_len = 4;
const static int packet_footer_len = 3;

std::string nsvr::config::HumanReadable(const Instruction & instruction)
{
	std::stringstream ss;
	ss << instruction.id._to_string() << " ";

	if (instruction.params.empty()) {
		ss << "[no parameters]";
	}
	else {
		ss << "[ ";
		for (auto param : instruction.params)
		{
			ss << '{' << param.first << " = " << std::to_string(param.second) << "} ";
		}
		ss << "]";
	}
	return ss.str();
}

std::vector<uint8_t> nsvr::config::Build(const Instruction & instruction)
{
	const auto packetLength = instruction.params.size() + packet_header_len + packet_footer_len;
	assert(packetLength <= 255);

	std::vector<uint8_t> packet(packetLength, 0);
	packet[0] = 0x24;
	packet[1] = 0x02;
	packet[2] = instruction.id;
	packet[3] = static_cast<uint8_t>(packetLength);

	for (std::size_t i = 0; i < instruction.params.size(); i++) {
		packet[i + 4] = instruction.params[i].second;
	}

	packet[packetLength - 3] = 0xFF;
	packet[packetLength - 2] = 0xFF;
	packet[packetLength - 1] = 0x0A;

	return packet;
}

bool nsvr::config::Verify(const Instruction & instruction, const InstructionSet* instructionSet)
{
	auto& instructionDict = instructionSet->Instructions();

	auto it = instructionDict.find(instruction.id._to_string());

	//First, the instruction must exist in our provided config file
	if (it == instructionDict.end()) {
		return false;
	}

	//Second, the instruction ID must correspond to the one in the config
	if (instruction.id != it->second.ByteId) {
		return false;
	}

	//Third, we must specify the same number of params as in the config
	if (instruction.params.size() != it->second.Parameters.size()) {
		return false;
	}

	//Actual verification of parameters is not yet implemented in this function, see InstructionBuilder.

	return true;
}
