#pragma once

#include "PluginAPI.h"

struct nsvr_waveform {
	nsvr_default_waveform waveform_id;
	float strength;
	std::size_t repetitions;
};


