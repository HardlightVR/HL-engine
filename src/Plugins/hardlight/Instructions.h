#pragma once

#include "better_enum.h"
#include <cstdint>
#include <vector>
#include "FirmwareInterface.h"
#include "PacketVersion.h"
class InstructionSet;

namespace nsvr {
	namespace config {

		BETTER_ENUM(InstructionId, uint8_t,
			GET_VERSION = 0x01,
			STATUS_PING = 0x02,
			GET_UUID = 0x03,
			SET_LEDS = 0x05,
			INITIALIZE_ALL = 0x12,
			PLAY_EFFECT = 0x13,
			HALT_SINGLE = 0x17,
			PLAY_CONTINUOUS = 0x19,
			INTRIG_MODE_ENABLE = 0x1B,
			RTP_MODE_ENABLE = 0x1C,
			PLAY_RTP = 0x1D,
			INIT_TRACKING = 0x32,
			IMU_ENABLE = 0x34,
			IMU_DISABLE = 0x35
		);


		struct Instruction {
			using Param = std::pair<std::string, uint8_t>;
			Instruction(InstructionId id, PacketVersion version, std::vector<Param> params) : id(id), params(std::move(params)), version(version) {}
			InstructionId id;
			std::vector<Param> params;
			PacketVersion version;
		};

		std::string HumanReadable(const Instruction& instruction);

		//Hmm. There's actually a problem here, because for instance a uint8_t effect or Location location is not actually the same
		//byte representation that is sent down the wire. They go through the translator -> string first, and then that string is looked up
		//in the config file for the correct byte value.

		//
		std::vector<uint8_t> Build(const Instruction& instruction);
		bool Verify(const Instruction& instruction, const InstructionSet* instructionSet);



	}
}