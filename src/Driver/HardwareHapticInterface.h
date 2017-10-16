#pragma once
#include "HapticInterface.h"
#include "PluginApis.h"


//Need to think about the idea of a plugin supporting both APIs at the boundary level, 
//but in fact only some devices/nodes support that specific api.
//So who makes sure than an invalid request isn't sent?

class HardwareWaveform {
public:
	HardwareWaveform(waveform_api* waveformApi);
	void SubmitSimpleHaptic(uint64_t request_id, nsvr_node_id id, SimpleHaptic data);
private:
	waveform_api* m_waveform;
};

