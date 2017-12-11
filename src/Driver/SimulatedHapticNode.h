#pragma once
#include <vector>
#include <stdint.h>
#include "PluginAPI.h"
#include "RenderedRegion.h"
#include <boost/optional.hpp>
const double MIN_CANCELLABLE_RESOLUTION = 0.25f;
class Waveform {
public:
	using Id = uint64_t;
	enum class PlaybackState {
		Unknown = 0,
		Playing = 1,
		Paused =2
	};
	Waveform(Id id, nsvr_default_waveform waveform, double strength, unsigned int repetitions);
	Waveform(Id id, const double* samples, double sampleDuration, std::size_t length);
	void update(double dt);
	double elapsed() const;
	double duration() const;
	std::pair<float, boost::optional<nsvr_default_waveform>> sample() const;
	Id id() const;

	void pause();
	void resume();
private:
	double m_sampleDuration;
	double m_elapsed;
	Id m_id;
	std::vector<double> m_samples;
	PlaybackState m_playbackState;
	boost::optional<nsvr_default_waveform> m_waveForm;
	double computeAmplitude() const;
};

class SimulatedHapticNode {
public:
	enum class PlaybackCommand {
		Pause,
		Resume,
		Cancel
	};
	using Id = uint64_t;
	void submitPlayback(Id id, PlaybackCommand command);

	void submitHaptic(Waveform waveform);
	void update(double dt);
	RenderedNode::GenericData render() const;
	
private:
	std::pair<float, boost::optional<nsvr_default_waveform>> sample() const;
	std::vector<Waveform> m_activeEffects;

};