#pragma once

#include "better_enum.h"
#include <cstdint>
#include <vector>

class InstructionSet;

namespace nsvr {
	namespace config {

		BETTER_ENUM(InstructionId, uint8_t,
			GET_VERSION = 0x01,
			STATUS_PING = 0x02,
			PLAY_CONTINUOUS = 0x19
		);


		struct Instruction {
			using Param = std::pair<std::string, uint8_t>;
			Instruction(InstructionId id, std::vector<Param> params) : id(id), params(std::move(params)) {}
			InstructionId id;
			std::vector<Param> params;
		};

		std::string HumanReadable(const Instruction& instruction);

		std::vector<uint8_t> Build(const Instruction& instruction);
		bool Verify(const Instruction& instruction, const InstructionSet* instructionSet);



	}
}