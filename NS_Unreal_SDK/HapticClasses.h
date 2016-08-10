#pragma once
#include "Enums.h"
#include <vector>

class HapticEffect {
public:
	HapticEffect(Effect effect, Location loc, float duration, float time, unsigned int priority);
	HapticEffect();
	~HapticEffect();
	Effect Effect;
	Location Location;
	float Duration;
	float Time;
	float OriginalTime;
	unsigned int Priority;
};

class HapticSequence
{
public:
	HapticSequence(std::vector<HapticEffect> effects);
	~HapticSequence();
	std::vector<HapticEffect> Effects;
};

class HapticFrame
{
public:
	HapticFrame(float time, std::vector<HapticSequence> frame, unsigned int priority = 1);
	~HapticFrame();
	float Time;
	float OriginalTime;
	unsigned int Priority;
	std::vector<HapticSequence> Frame;
};

class HapticSample
{
public:
	unsigned int Priority;
	float Time;
	float OriginalTime;
	std::vector<HapticFrame> Frames;
	HapticSample(float time, std::vector<HapticFrame> frames, unsigned int priority);
	~HapticSample();
};

class Moment
{
public:
	std::string Name;
	float Time;
	Side Side;
	Moment(std::string name, float t, ::Side side = Side::Inherit);
	~Moment();
};
