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
			GET_PING = 0x02,
			GET_UUID = 0x03,
			SET_LEDS = 0x05,
			GET_MOTOR_STATUS = 0x10,
			SET_MOTOR_INIT = 0x11,
			SET_MOTOR_INIT_ALL = 0x12,
			SET_MOTOR_PLAY_EFFECT = 0x13,
			SET_MOTOR_DATA = 0x14,
			GET_MOTOR_DATA = 0x15,
			SET_MOTOR_LOAD_CONTINUOUS = 0x16,
			SET_MOTOR_HALT_SINGLE = 0x17,
			SET_MOTOR_STREAM_CONTINUOUS = 0x18,
			SET_MOTOR_PLAY_CONTINUOUS = 0x19,
			SET_MOTOR_PLAY_AUDIO = 0x1a,
			SET_MOTOR_INTRIGMODE = 0x1B,
			SET_MOTOR_RTPMODE = 0x1C,
			SET_MOTOR_PLAY_RTP = 0x1D,
			SET_MOTOR_WAVEFORM = 0x1e,
			SET_MOTOR_GO = 0x1f,
			SET_MOTOR_STOP_AUDIO = 0x22,
			GET_TRACK_STATUS = 0x30,
			SET_TRACK_INIT = 0x31,
			SET_TRACK_INIT_ALL = 0x32,
			GET_TRACK_DATA = 0x33,
			SET_TRACK_ENABLE = 0x34,
			SET_TRACK_DISABLE = 0x35,
			GET_TRACK_GRAVITY = 0x36,
			GET_TRACK_MAG = 0x37,
			GET_TRACK_UUID = 0x38
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