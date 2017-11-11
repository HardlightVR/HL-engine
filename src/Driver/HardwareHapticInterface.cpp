#include "stdafx.h"
#include "HardwareHapticInterface.h"
#include "WaveformGenerators.h"
#include "nsvr_preset.h"
HardwareHapticInterface::HardwareHapticInterface()
	: m_waveform(nullptr)
	, m_buffered(nullptr)
{

}
void HardwareHapticInterface::Submit(uint64_t request_id, nsvr_node_id node_id, const WaveformData & data)
{

	if (m_waveform) {
		m_waveform->submit_activate(request_id, node_id, static_cast<nsvr_default_waveform>(data.waveform), data.repetitions, data.strength);
	}
	else if (m_buffered) {
		double sample_duration_ms = 0;
		m_buffered->submit_getsampleduration(node_id, &sample_duration_ms);

		const double default_waveform_avg_duration_ms = 250;
		const int how_many_samples_we_need = default_waveform_avg_duration_ms / sample_duration_ms;
		auto samples = nsvr::waveforms::generateWaveform(data.strength, static_cast<nsvr_default_waveform>(data.waveform), how_many_samples_we_need);
		m_buffered->submit_buffer(request_id, node_id, samples.data(), samples.size());
	}
}


std::vector<double> resample(const std::vector<double>& source, float freq, double intended_sample_duration_ms) {

	float inputSampleDurationMs = (1.0f / freq) * 1000; //30Hz - 33.33 ms
	float totalInputDurationMs = source.size() * inputSampleDurationMs; //say, 10 samples: 333.3 ms total duration

	float totalOutputSamples = totalInputDurationMs / intended_sample_duration_ms; // 333.3 / 20ms = 16.5 samples
	std::vector<double> out;
	out.reserve(totalOutputSamples);
	for (int i = 0; i < totalOutputSamples; i++) {
		float fraction = i / totalOutputSamples;
		int other_fraction = fraction * source.size();
		out.push_back(source.at(other_fraction));
	}

	return out;



}
void HardwareHapticInterface::Submit(uint64_t request_id, nsvr_node_id node_id, const BufferedData & data)
{
	if (m_buffered) {
		double sample_duration_ms = 0;
		m_buffered->submit_getsampleduration(node_id, &sample_duration_ms);

		auto newSamples = resample(data.samples, data.frequency, sample_duration_ms);

		m_buffered->submit_buffer(request_id, node_id,newSamples.data(), newSamples.size());
	}
}
void HardwareHapticInterface::Submit(uint64_t request_id, nsvr_node_id, const ContinuousData & data)
{
}
void HardwareHapticInterface::ProvideWaveform(waveform_api * api)
{
	m_waveform = api;
}
void HardwareHapticInterface::ProvideBuffered(buffered_api * api)
{
	m_buffered = api;
}
//
//void HardwareHapticInterface::SubmitSimpleHaptic(uint64_t request_id, nsvr_node_id node_id, SimpleHaptic data)
//{
//	//eventually we will need to branch on whether this specific node can handle buffered or waveform, because it might be 
//	//that some nodes can only do buffered playback. Perhaps one layer up could check and then call the correct method here,
//	//splitting this into SubmitSimpleHaptic(req, id, data, mode::buffered) or something.
//
//	
//		nsvr_waveform wave{};
//		wave.repetitions = static_cast<std::size_t>(data.duration / 0.25f);
//		wave.strength = data.strength;
//		wave.waveform_id = static_cast<nsvr_default_waveform>(data.effect_preset);
//		m_waveform->submit_activate(request_id, node_id, reinterpret_cast<nsvr_waveform*>(&wave));
//	/*
//	else if (m_buffered) {
//		auto samples = nsvr::waveforms::generateWaveform(data.strength, static_cast<nsvr_default_waveform>(data.effect_preset));
//		m_buffered->submit_buffer(request_id, node_id, samples.data(), samples.size());
//	}*/
//}

