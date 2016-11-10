#pragma once
#include <unordered_map>
#include <memory>
#include "HapticClasses.h"
#include "flatbuff_defs\HapticPacket_generated.h"

 
template<typename Sequence, typename Pattern, typename Experience>
class HapticCache2
{
public:
	HapticCache2();
	~HapticCache2();
	void AddSequence(std::string name, Sequence effects);
	void AddPattern(std::string name, Pattern frames);
	void AddExperience(std::string name, Experience samples);
	bool ContainsSequence(std::string name);
	bool ContainsPattern(std::string name);
	bool ContainsExperience(std::string name);
	Sequence GetSequence(std::string name);
	Pattern GetPattern(std::string name);
	Experience GetExperience(std::string name);
	void Clear();
private:
	std::unordered_map<std::string, Sequence> _sequences;
	std::unordered_map<std::string, Pattern> _patterns;
	std::unordered_map<std::string, Experience> _experiences;


};

template<typename Sequence, typename Pattern, typename Experience>
HapticCache2<Sequence, Pattern, Experience>::HapticCache2()
{
}

template<typename Sequence, typename Pattern, typename Experience>
HapticCache2<Sequence, Pattern, Experience>::~HapticCache2()
{
}

template<typename Sequence, typename Pattern, typename Experience>
void HapticCache2<Sequence, Pattern, Experience>::AddSequence(std::string name, Sequence effects)
{
	_sequences[name] = effects;
}

template<typename Sequence, typename Pattern, typename Experience>

void HapticCache2<Sequence, Pattern, Experience>::AddPattern(std::string name, Pattern frames)
{
	_patterns[name] = frames;
}
template<typename Sequence, typename Pattern, typename Experience>

void HapticCache2<Sequence, Pattern, Experience>::AddExperience(std::string name, Experience samples)
{
	_experiences[name] = samples;
}


template<typename Sequence, typename Pattern, typename Experience>

bool HapticCache2<Sequence, Pattern, Experience>::ContainsSequence(std::string name)
{
	return _sequences.find(name) != _sequences.end();
}
template<typename Sequence, typename Pattern, typename Experience>

bool HapticCache2<Sequence, Pattern, Experience>::ContainsPattern(std::string name)
{
	return _patterns.find(name) != _patterns.end();
}
template<typename Sequence, typename Pattern, typename Experience>

bool HapticCache2<Sequence, Pattern, Experience>::ContainsExperience(std::string name)
{
	return _experiences.find(name) != _experiences.end();
}


template<typename Sequence, typename Pattern, typename Experience>
void HapticCache2<Sequence, Pattern, Experience>::Clear()
{
	_patterns.clear();
	_experiences.clear();
	_sequences.clear();
}

template<typename Sequence, typename Pattern, typename Experience>
Sequence HapticCache2<Sequence, Pattern, Experience>::GetSequence(std::string name) {
	return _sequences.at(name);
}

template<typename Sequence, typename Pattern, typename Experience>
Pattern HapticCache2<Sequence, Pattern, Experience>::GetPattern(std::string name) {
	return _patterns.at(name);
}

template<typename Sequence, typename Pattern, typename Experience>
Experience HapticCache2<Sequence, Pattern, Experience>::GetExperience(std::string name) {
	return _experiences.at(name);
}