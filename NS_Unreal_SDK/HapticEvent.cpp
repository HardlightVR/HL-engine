#include "HapticEvent.h"



HapticEvent::HapticEvent(::Effect effect, float duration):Dirty(false), Effect(effect), Duration(duration), TimeElapsed(0)
{
}

HapticEvent::~HapticEvent()
{
}

::Duration HapticEvent::DurationType() const
{
	if (Duration > 0)
	{
		return ::Duration::Variable;
	}
	//cast to int
	return ::Duration(Duration);
}
