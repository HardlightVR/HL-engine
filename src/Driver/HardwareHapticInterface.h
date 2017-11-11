#pragma once
#include "PluginApis.h"

struct WaveformData {
	uint32_t repetitions;
	float strength;
	uint32_t waveform;
};

struct ContinuousData {
	float strength;
};

struct BufferedData {
	std::vector<double> samples;
	float frequency;
};

//Need to think about the idea of a plugin supporting both APIs at the boundary level, 
//but in fact only some devices/nodes support that specific api.
//So who makes sure than an invalid request isn't sent?

std::vector<double> resample(const std::vector<double>& source, float freq, double intended_sample_duration_ms);

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

