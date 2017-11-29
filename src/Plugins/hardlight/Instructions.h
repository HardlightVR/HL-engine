#pragma once


#include <cstdint>
#include <vector>
#include <tuple>


#include "better_enum.h"


namespace inst {


BETTER_ENUM(Id, uint8_t,
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
template<typename T>
struct param {
	uint8_t value;
	explicit param(uint8_t val) : value(val) {}
};

#define PARAM(name) struct name##_t {}; using name = param<name##_t>

PARAM(effect);
PARAM(red);
PARAM(blue);
PARAM(green);
PARAM(motor);
PARAM(reg);
PARAM(volume);
PARAM(waveform);
PARAM(sensor);
PARAM(audio_vibe);
PARAM(audio_minlevel);
PARAM(audio_maxlevel);
PARAM(audio_mindrv);
PARAM(audio_maxdrv);






template<std::size_t i = 0, typename ...Tp>
inline typename std::enable_if<i == sizeof...(Tp), void>::type
	for_each(const std::tuple<Tp...>&, uint8_t* data) {}

template<std::size_t i = 0, typename ...Tp>
inline typename std::enable_if<i < sizeof...(Tp), void>::type
	for_each(const std::tuple<Tp...>& t, uint8_t* data) {
	data[i] = std::get<i>(t).value;
	for_each<i + 1, Tp...>(t, data);
}


template<std::size_t instruction_id, typename...Args>
struct instruction {
	std::tuple<Args...> args;
	uint8_t id = instruction_id;
	instruction(Args... args) : args(std::forward<Args>(args)...) {
		static_assert(std::tuple_size<std::tuple<Args...>>::value <= 9, "cannot have more than 9 arguments");

	}
	void serialize(uint8_t* inputBuffer) const {
		for_each(args, inputBuffer);
	}
};

#define INST(id, name, ...) using name = instruction<id, __VA_ARGS__>

INST(Id::GET_VERSION, get_version);
INST(Id::GET_PING, get_ping);
INST(Id::GET_UUID, get_uuid);
INST(Id::SET_LEDS, set_leds, red, red, green, green, blue, blue);
INST(Id::GET_MOTOR_STATUS, get_motor_status, motor);
INST(Id::SET_MOTOR_PLAY_EFFECT, set_motor_play_effect, motor, effect);
INST(Id::SET_MOTOR_HALT_SINGLE, set_motor_halt_single, motor);
INST(Id::SET_MOTOR_PLAY_CONTINUOUS, set_motor_play_continuous, motor, effect);
INST(Id::SET_MOTOR_PLAY_AUDIO, set_motor_audiomode, motor, audio_vibe, audio_minlevel, audio_maxlevel, audio_mindrv, audio_maxdrv);
INST(Id::SET_MOTOR_STOP_AUDIO, set_motor_stop_audio, motor);
INST(Id::SET_MOTOR_INTRIGMODE, set_motor_intrigmode, motor);
INST(Id::SET_MOTOR_RTPMODE, set_motor_rtpmode, motor);
INST(Id::SET_MOTOR_PLAY_RTP, set_motor_play_rtp, motor, volume);
INST(Id::GET_TRACK_STATUS, get_track_status, sensor);
INST(Id::SET_TRACK_ENABLE, set_track_enable);
INST(Id::SET_TRACK_DISABLE, set_track_disable);
INST(Id::GET_TRACK_GRAVITY, get_track_gravity, sensor);
INST(Id::GET_TRACK_MAG, get_track_compass, sensor);
INST(Id::GET_TRACK_DATA, get_track_data, sensor);

template<typename Instruction>
std::vector<uint8_t> Build(const Instruction & instruction)
{

	constexpr uint8_t packetLength = 16;

	std::vector<uint8_t> packet(packetLength, 0);

	packet[0] = 0x24;
	packet[1] = 0x02;
	packet[2] = instruction.id;
	packet[3] = static_cast<uint8_t>(packetLength);

	instruction.serialize(packet.data() + 4);



	packet[packetLength - 3] = 0xFF;
	packet[packetLength - 2] = 0x0D;
	packet[packetLength - 1] = 0x0A;

	return packet;
}

}


		