#pragma once

#include "PluginAPI.h"

#include "PluginApis.h"
struct SimpleHaptic {
	float duration;
	float strength;
	uint32_t effect_preset;

	SimpleHaptic(uint32_t effect_preset, float duration, float strength);
};


class HapticInterface {
public:
	HapticInterface(buffered_api* bufferedApi, waveform_api* waveformApi);
	void SubmitSimpleHaptic(uint64_t request_id, nsvr_node_id id, SimpleHaptic data);
private:
	buffered_api* m_buffered;
	waveform_api* m_waveform;
};