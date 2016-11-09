#pragma once
#include "Enums.h"

class HapticEvent
{
public:
	HapticEvent(Effect effect, float duration);
	~HapticEvent();
	boost::uuids::uuid Handle;
	bool Dirty;
	Effect Effect;
	float Duration;
	float TimeElapsed;
	bool Sent;
	::Duration DurationType() const;
	bool operator==(const HapticEvent& rhs);
};

