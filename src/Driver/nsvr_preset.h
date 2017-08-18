#pragma once

#include "PluginAPI.h"

struct nsvr_preset_request {
	nsvr_preset_family family;
	float strength;
	double duration;
	uint64_t handle;
};
	
