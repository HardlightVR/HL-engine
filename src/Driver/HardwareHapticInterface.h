#pragma once
#include "HapticInterface.h"
#include "PluginApis.h"

class HardwareHapticInterface : public HapticInterface{
public:
	HardwareHapticInterface(buffered_api* bufferedApi, waveform_api* waveformApi);
	void SubmitSimpleHaptic(uint64_t request_id, nsvr_node_id id, SimpleHaptic data) override;
private:
	buffered_api* m_buffered;
	waveform_api* m_waveform;
};