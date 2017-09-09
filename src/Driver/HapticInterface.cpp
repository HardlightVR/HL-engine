#include "stdafx.h"
#include "HapticInterface.h"
#include "WaveformGenerators.h"
#include "nsvr_preset.h"
HapticInterface::HapticInterface(buffered_api * bufferedApi, waveform_api * waveformApi)
	: m_buffered(bufferedApi)
	, m_waveform(waveformApi)
{

}

void HapticInterface::SubmitSimpleHaptic(uint64_t request_id, nsvr_node_id node_id, SimpleHaptic data)
{
	//eventually we will need to branch on whether this specific node can handle buffered or waveform, because it might be 
	//that some nodes can only do buffered playback. Perhaps one layer up could check and then call the correct method here,
	//splitting this into SubmitSimpleHaptic(req, id, data, mode::buffered) or something.

	if (m_waveform) {
		nsvr_waveform wave{};
		wave.repetitions = static_cast<std::size_t>(data.duration / 0.25f);
		wave.strength = data.strength;
		wave.waveform_id = static_cast<nsvr_default_waveform>(data.effect_preset);
		m_waveform->submit_activate(request_id, node_id, reinterpret_cast<nsvr_waveform*>(&wave));
	}
	else if (m_buffered) {
		auto samples = nsvr::waveforms::generateWaveform(data.strength, static_cast<nsvr_default_waveform>(data.effect_preset));
		m_buffered->submit_buffer(request_id, node_id, samples.data(), samples.size());
	}
}

SimpleHaptic::SimpleHaptic(uint32_t effect_preset, float duration, float strength)
	: duration(duration)
	, strength(strength)
	, effect_preset(effect_preset)
{
}
