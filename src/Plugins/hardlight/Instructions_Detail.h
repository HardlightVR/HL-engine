#pragma once
#include <cstdint>

namespace inst {

	template<typename T>
	struct param {
		uint8_t value;
		explicit param(uint8_t val) : value(val) {}
	};

	#define PARAM(name) struct name##_t {}; using name = param<name##_t>


	//This is necessary to do packet serialization
	//We basically loop over N tuple values, assigning them to values in an array
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


}