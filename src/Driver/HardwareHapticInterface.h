#pragma once
#include "PluginApis.h"

struct WaveformData {
	float duration;
	float strength;
	uint32_t waveform;
};

struct ContinuousData {
	float strength;
};

struct BufferedData {

};

//Need to think about the idea of a plugin supporting both APIs at the boundary level, 
//but in fact only some devices/nodes support that specific api.
//So who makes sure than an invalid request isn't sent?

class HardwareHapticInterface {
public:
	HardwareHapticInterface();
	void Submit(uint64_t request_id, nsvr_node_id, const WaveformData& data);
	void Submit(uint64_t request_id, nsvr_node_id, const BufferedData& data);
	void Submit(uint64_t request_id, nsvr_node_id, const ContinuousData& data);

	void ProvideWaveform(waveform_api* api);
	void ProvideBuffered(buffered_api* api);
private:
	waveform_api* m_waveform;
	buffered_api* m_buffered;
};

