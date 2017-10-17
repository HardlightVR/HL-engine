#include "stdafx.h"
#include "WaveformGenerators.h"

namespace nsvr {
namespace waveforms {


void sinsample(std::vector<double>& samples, float maxStrength, std::size_t numSamples) {
	float clampedStrength = std::max(0.0f, std::min(1.0f, maxStrength));

	float total = static_cast<float>(M_PI / numSamples);
	for (std::size_t i = 0; i < numSamples; i++) {

		float s = std::sin((float)i*total)*clampedStrength;
		if (s > 0) {
			samples.push_back(s);
		}
		else {
			samples.push_back(0);
		}
	}

}

void constant(std::vector<double>& samples, float strength, std::size_t numsamples) {
	float clampedStrength = std::max(0.0f, std::min(1.0f, strength));
	for (std::size_t i = 0; i < numsamples; i++) {
		samples.push_back(clampedStrength);
	}
}
std::vector<double> generateWaveform(float strength, nsvr_default_waveform family) {
	if (nsvr_preset_family_click == family) {
		std::vector<double> samples;
		sinsample(samples, strength, 10);
		return samples;
	}
	else if (nsvr_preset_family_pulse == family) {
		std::vector<double> samples;
		sinsample(samples, strength, 50);
		constant(samples, 0, 5);

		return samples;
	}
	else if (nsvr_preset_family_tick == family) {
		std::vector<double> samples;
		constant(samples, strength, 5);
		return samples;
	}
	else if (nsvr_preset_family_buzz == family) {
		std::vector<double> samples;
		constant(samples, strength, 5);
		constant(samples, 0, 3);
		constant(samples, strength, 5);
		constant(samples, 0, 3);
		constant(samples, strength, 5);
		constant(samples, 0, 3);
		constant(samples, strength, 5);
		constant(samples, 0, 3);
		constant(samples, strength, 5);
		constant(samples, 0, 3);
		constant(samples, strength, 5);
		constant(samples, 0, 3);
		return samples;
	}
	else if (nsvr_preset_family_double_click == family) {
		std::vector<double> samples;
		sinsample(samples, strength, 8);
		constant(samples, 0.0f, 26);
		sinsample(samples, strength, 8);
		constant(samples, 0.0f, 20);

		return samples;
	}
	else if (nsvr_preset_family_triple_click == family) {
		std::vector<double> samples;
		sinsample(samples, strength, 8);
		constant(samples, 0.0f, 26);
		sinsample(samples, strength, 8);
		constant(samples, 0.0f, 20);
		sinsample(samples, strength, 8);
		constant(samples, 0.0f, 20);
		return samples;
	}
	else if (nsvr_preset_family_hum == family) {
		std::vector<double> samples;
		constant(samples, strength, 62);
		return samples;
	}

	else {
		std::vector<double> samples;
		constant(samples, strength, 8);
		return samples;
	}

}



}
}