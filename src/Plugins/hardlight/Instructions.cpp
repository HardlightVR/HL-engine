#include "stdafx.h"
#include "Instructions.h"
#include <cassert>
#include "InstructionSet.h"
#include <sstream>

const static int packet_header_len = 4;
const static int packet_footer_len = 3;
//
//std::string nsvr::config::HumanReadable(const Instruction & instruction)
//{
//	std::stringstream ss;
//	ss << instruction.id._to_string() << " ";
//
//	if (instruction.params.empty()) {
//		ss << "[no parameters]";
//	}
//	else {
//		ss << "[ ";
//		for (auto param : instruction.params)
//		{
//			ss << '{' << param.first << " = " << std::to_string(param.second) << "} ";
//		}
//		ss << "]";
//	}
//	return ss.str();
//}
