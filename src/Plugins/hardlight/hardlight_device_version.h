#pragma once
#include <cstdint>
#include "suit_packet.h"

struct hardlight_device_version {
	int product;
	int revision;
	int firmware_a;
	int firmware_b;
};

inline hardlight_device_version parse_version(Packet packet) {
	return hardlight_device_version{ packet[3], packet[4], packet[5], packet[6] };
}


