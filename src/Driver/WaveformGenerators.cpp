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
std::vector<double> generateWaveform(float strength, nsvr_default_waveform family, int how_many) {
	if (nsvr_preset_family_click == family) {
		std::vector<double> samples;
		sinsample(samples, strength, how_many);
		return samples;
	}
	else if (nsvr_preset_family_pulse == family) {
		std::vector<double> samples;
		const int ten_percent = how_many * .1;
		const int ninety_percent = how_many * .9;
		sinsample(samples, strength, ninety_percent);
		constant(samples, 0, ten_percent);

		return samples;
	}
	else if (nsvr_preset_family_tick == family) {
		std::vector<double> samples;
		constant(samples, strength, 5);
		return samples;
	}
	else if (nsvr_preset_family_buzz == family) {

		const int ten_percent = how_many * .1;
		const int six_percent = how_many * .06;
		std::vector<double> samples;
		for (int i = 0; i < 6; i++) {
			constant(samples, strength, ten_percent);
			constant(samples, 0, six_percent);
		}

		return samples;
	}
	else if (nsvr_preset_family_double_click == family) {
		std::vector<double> samples;
		const int thirteen_percent = how_many * 0.13;
		const int fortytwo_percent = how_many * 0.42;
		const int thirtytwo_percent = how_many * 0.32;
		sinsample(samples, strength, thirteen_percent);
		constant(samples, 0.0f, fortytwo_percent);
		sinsample(samples, strength, thirteen_percent);
		constant(samples, 0.0f, thirtytwo_percent);

		return samples;
	}
	else if (nsvr_preset_family_triple_click == family) {
		std::vector<double> samples;
		const int one_third = how_many * .3;
		const int one_quarter = one_third * 0.25;
		const int three_quarter = one_third * 0.75;
		for (int i = 0; i < 3; i++) {
			sinsample(samples, strength, one_quarter);
			constant(samples, 0.0f, three_quarter);
		}
		return samples;
	}
	else if (nsvr_preset_family_hum == family) {
		std::vector<double> samples;
		constant(samples, strength, how_many);
		return samples;
	}

	else {
		std::vector<double> samples;
		constant(samples, strength, how_many);
		return samples;
	}

}



}
}