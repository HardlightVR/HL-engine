#pragma once

#include <vector>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>

#include <PluginAPI.h>
namespace nsvr {
namespace waveforms {


	void sinsample(std::vector<double>& samples, float maxStrength, std::size_t numSamples);
	void constant(std::vector<double>& samples, float strength, std::size_t numsamples);
	std::vector<double> generateWaveform(float strength, nsvr_default_waveform family, int how_many);

}}