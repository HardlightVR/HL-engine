#pragma once

#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include <PluginAPI.h>
namespace nsvr {
namespace waveforms {


	std::vector<double> sin_sample(float strength, std::size_t numSamples);
	std::vector<double> repeated(float strength, std::size_t numSamples);

	std::vector<double> generateWaveform(float strength, nsvr_default_waveform family, int how_many);

	void append(std::vector<double>& samples, const std::vector<double>& newSamples);


}}