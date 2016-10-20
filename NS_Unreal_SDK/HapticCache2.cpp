#include "HapticCache2.h"



HapticCache2::HapticCache2()
{
}


HapticCache2::~HapticCache2()
{
}


void HapticCache2::AddSequence(std::string name, std::vector<HapticEffect> effects)
{
	_sequences[name] = effects;
}

void HapticCache2::AddPattern(std::string name, std::vector<HapticFrame> frames)
{
	_patterns[name] = frames;
}

void HapticCache2::AddExperience(std::string name, std::vector<HapticSample> samples)
{
	_experiences[name] = samples;	
}



bool HapticCache2::ContainsSequence(std::string name)
{
	return _sequences.find(name) != _sequences.end();
}

bool HapticCache2::ContainsPattern(std::string name)
{
	return _patterns.find(name) != _patterns.end();
}

bool HapticCache2::ContainsExperience(std::string name)
{
	return _experiences.find(name) != _experiences.end();
}

std::vector<HapticEffect> HapticCache2::GetSequence(std::string name)
{
	return _sequences.at(name);
}

std::vector<HapticFrame> HapticCache2::GetPattern(std::string name)
{
	return _patterns.at(name);
}

std::vector<HapticSample> HapticCache2::GetExperience(std::string name)
{
	return _experiences.at(name);
}

void HapticCache2::Clear()
{
	_patterns.clear();
	_experiences.clear();
	_sequences.clear();
}
