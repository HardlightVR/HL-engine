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
