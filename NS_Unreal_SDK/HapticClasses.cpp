#include "HapticClasses.h"

HapticEffect::HapticEffect(::Effect effect, ::Location loc, float duration, float time, unsigned int priority):
	Effect(effect),
	Location(loc),
	Duration(duration),
	Time(time),
	OriginalTime(time),
	Priority(priority)
{
}



HapticEffect::~HapticEffect()
{
}

float HapticEffect::GetTime()
{
	return Time;
}

HapticSequence::HapticSequence(std::vector<HapticEffect> effects):Effects(effects)
{
}

HapticSequence::~HapticSequence()
{
}


HapticSample::HapticSample(float time, std::vector<HapticFrame> frames, unsigned int priority):
Priority(priority), Time(time), OriginalTime(time), Frames(frames)

{
}

HapticSample::~HapticSample()
{
}


float Moment::GetTime()
{
	return Time;
}

Moment::Moment(std::string name, float t, ::Side side):Name(name), Time(t), Side(side)
{
}

Moment::Moment():Time(0), Side(Side::NotSpecified)
{
}

Moment::~Moment()
{
}
