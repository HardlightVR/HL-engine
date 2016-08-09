#include "HapticClasses.h"


HapticEffect::HapticEffect(::Effect effect, ::Location loc, float duration, float time, unsigned priority):
	Effect(effect),
	Location(loc),
	Duration(duration),
	Time(time),
	OriginalTime(time),
	Priority(priority)
{
}

HapticEffect::HapticEffect()
{
}

HapticEffect::~HapticEffect()
{
}

HapticSequence::HapticSequence(std::vector<HapticEffect> effects):Effects(effects)
{
}

HapticSequence::~HapticSequence()
{
}

HapticFrame::HapticFrame(float time, std::vector<HapticSequence> frame, unsigned priority): Time(time), OriginalTime(time), Priority(priority), Frame(frame)
{
}

HapticFrame::~HapticFrame()
{
}
