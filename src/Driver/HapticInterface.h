#pragma once

#include "PluginAPI.h"


struct SimpleHaptic {
	float duration;
	float strength;
	uint32_t effect_preset;

	SimpleHaptic(uint32_t effect_preset, float duration, float strength) : duration(duration)
		, strength(strength)
		, effect_preset(effect_preset) {}
};

class HapticInterface {
public:
	virtual void SubmitSimpleHaptic(uint64_t request_id, nsvr_node_id id, SimpleHaptic data) = 0;

};