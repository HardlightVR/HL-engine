#pragma once
#include "Enums.h"
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

