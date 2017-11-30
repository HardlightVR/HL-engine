#include "stdafx.h"
#include "WaveformGenerators.h"

namespace nsvr {
namespace waveforms {

void append(std::vector<double>& samples, const std::vector<double>& newSamples) {

	samples.insert(samples.end(), newSamples.begin(), newSamples.end());

}

std::vector<double> sin_sample(float strength, std::size_t numSamples) {
	float clampedStrength = std::max(0.0f, std::min(1.0f, strength));
	std::vector<double> samples;
	samples.reserve(numSamples);

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
	return samples;
}

std::vector<double> repeated(float strength, std::size_t numSamples) {
	std::vector<double> samples;
	samples.reserve(numSamples);
	float clampedStrength = std::max(0.0f, std::min(1.0f, strength));
	for (std::size_t i = 0; i < numSamples; i++) {
		samples.push_back(clampedStrength);
	}
	return samples;
}

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
		append(samples, sin_sample(strength, ninety_percent));
		append(samples, repeated(0, ten_percent));
		return samples;
	}
	else if (nsvr_preset_family_tick == family) {
		return repeated(strength, 5);
	}
	else if (nsvr_preset_family_buzz == family) {

		const int ten_percent = how_many * .1;
		const int six_percent = how_many * .06;
		std::vector<double> samples;
		for (int i = 0; i < 6; i++) {
			append(samples, repeated(strength, ten_percent));
			append(samples, repeated(0, six_percent));
		}

		return samples;
	}
	else if (nsvr_preset_family_double_click == family) {
		std::vector<double> samples;
		const int thirteen_percent = how_many * 0.13;
		const int fortytwo_percent = how_many * 0.42;
		const int thirtytwo_percent = how_many * 0.32;

		append(samples, sin_sample(strength, thirteen_percent));
		append(samples, repeated(0, fortytwo_percent));
		append(samples, sin_sample(strength, thirteen_percent));
		append(samples, repeated(0, thirtytwo_percent));
		return samples;
	}
	else if (nsvr_preset_family_triple_click == family) {
		std::vector<double> samples;
		const int one_third = how_many * .3;
		const int one_quarter = one_third * 0.25;
		const int three_quarter = one_third * 0.75;
		for (int i = 0; i < 3; i++) {
			append(samples, sin_sample(strength, one_quarter));
			append(samples, repeated(0, three_quarter));
		}
		return samples;
	}
	else if (nsvr_preset_family_hum == family) {
		return repeated(strength, how_many);
	}

	else {
		return repeated(strength, how_many);
	}

}



}
}