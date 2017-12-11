#include "stdafx.h"
#include "SimulatedHapticNode.h"

#include "WaveformGenerators.h"

//Right now, nothing specific for click, hum, etc. Just a quarter second sample at the given strength
Waveform::Waveform(SimulatedHapticNode::Id id, nsvr_default_waveform waveform, double strength, unsigned int repetitions)
	: m_sampleDuration(20)
	, m_samples(nsvr::waveforms::generateWaveform(static_cast<float>(strength), waveform, 10))
	, m_elapsed(0)
	, m_id(id)
	, m_playbackState(PlaybackState::Playing)
	, m_waveForm(waveform)
{
	//auto samples = nsvr::waveforms::generateWaveform(static_cast<float>(strength), waveform);
	//for (int i = 0; i < repetitions; i++) {
//		m_samples.insert(m_samples.end(), samples.begin(), samples.end());
	//}
}

Waveform::Waveform(SimulatedHapticNode::Id id, const double * samples, double sampleDurationMs, std::size_t length)
	: m_sampleDuration(sampleDurationMs)
	, m_samples(samples, samples+length)
	, m_elapsed(0)
	, m_id(id)
	, m_playbackState(PlaybackState::Playing)
	, m_waveForm()
{
}

void Waveform::update(double fractionalSeconds)
{
	if (m_playbackState == PlaybackState::Playing) {
		m_elapsed += fractionalSeconds * 1000;
	}
}

double Waveform::elapsed() const
{
	return m_elapsed;
}

double Waveform::duration() const
{
	return m_sampleDuration * m_samples.size();
}

std::pair<float, boost::optional<nsvr_default_waveform>> Waveform::sample() const
{
	switch (m_playbackState) {
	case PlaybackState::Paused:
		return std::make_pair(0.0f, m_waveForm);
	case PlaybackState::Playing:
		return std::make_pair(static_cast<float>(computeAmplitude()), m_waveForm);
	default:
		return std::make_pair(0.0f, m_waveForm);
	}
}

SimulatedHapticNode::Id Waveform::id() const
{
	return m_id;
}

void Waveform::pause()
{
	m_playbackState = PlaybackState::Paused;
}

void Waveform::resume()
{
	m_playbackState = PlaybackState::Playing;
}

double Waveform::computeAmplitude() const
{
	//1.0 second elapsed, with sampleDuration == 0.25 => 4
	//0.5 second elapsed, with sampleDuration == 0.25 => 2
	//0.5 second elapsed, with sampleDuration == 0.125 => 4
	std::size_t roughIndex = static_cast<std::size_t>(std::floor(m_elapsed / m_sampleDuration));
	assert(roughIndex < m_samples.size());
	return m_samples[roughIndex];

}

void SimulatedHapticNode::submitPlayback(Id id, PlaybackCommand command)
{
	switch (command) {
	case PlaybackCommand::Pause:
		for (auto& effect : m_activeEffects) {
			if (effect.id() == id) {
				effect.pause();
			}
		}
		break;
	case PlaybackCommand::Resume:
		for (auto& effect : m_activeEffects) {
			if (effect.id() == id) {
				effect.resume();
			}
		}
		break;
	case PlaybackCommand::Cancel:
		m_activeEffects.erase(
			std::remove_if(m_activeEffects.begin(), m_activeEffects.end(), 
				[id](const auto& e) {return e.id() == id && e.duration() > MIN_CANCELLABLE_RESOLUTION; }), m_activeEffects.end());
		break;
	default:
		break;
	}
	
}


void SimulatedHapticNode::submitHaptic(Waveform waveform)
{
	m_activeEffects.push_back(std::move(waveform));
}

void SimulatedHapticNode::update(double dt)
{
	for (auto& effect : m_activeEffects) {
		effect.update(dt);
	}

	auto toRemove = std::remove_if(m_activeEffects.begin(), m_activeEffects.end(), [](const auto& e) { return e.elapsed() >= e.duration(); });
	m_activeEffects.erase(toRemove, m_activeEffects.end());



}

RenderedNode::GenericData SimulatedHapticNode::render() const
{
	auto data = sample();
	float waveformTag = data.second ? static_cast<float>(*data.second) : 0.0f;

	return RenderedNode::GenericData{data.first, waveformTag, 0.0, 0.0 };
}

std::pair<float, boost::optional<nsvr_default_waveform>> SimulatedHapticNode::sample() const
{
	return m_activeEffects.empty()
		? std::make_pair(0.0f, boost::none)
		: m_activeEffects.back().sample();
}
