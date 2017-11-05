#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "Instructions.h"
#include <array>
#include <cstdint>

using namespace inst;

TEST_CASE("The new instructions build correctly", "[instruction]") {
	constexpr std::size_t PACKET_LEN = 16;
	using packet_t = std::array<uint8_t, PACKET_LEN>;

	packet_t packet;
	std::fill(packet.begin(), packet.end(), 0);

	const packet_t empty_packet = packet;


	REQUIRE(packet.size() == PACKET_LEN);

	SECTION("Instruction ID is correct") {
		auto emptyInst = instruction<12>();
		REQUIRE(emptyInst.id == 12);

		

	}

	SECTION("An empty instruction does not serialize anything") {
		auto emptyInst = instruction<25>();
		emptyInst.serialize(packet.data());
		REQUIRE(packet == empty_packet);
	}

	SECTION("An inst") {
		auto oneArg = instruction<25, motor>(motor{ 3 });
		oneArg.serialize(packet.data());
		REQUIRE(packet[0] == 3);

	}
	
	SECTION("An inst") {
		auto twoArgs = instruction<25, motor, effect>(motor{ 3 }, effect{ 12 });
		twoArgs.serialize(packet.data());
		REQUIRE(packet[0] == 3);
		REQUIRE(packet[1] == 12);


	}


}

int main(int argc, char* argv[]) {
	int result = Catch::Session().run(argc, argv);
	std::cin.get();
	return result;
}