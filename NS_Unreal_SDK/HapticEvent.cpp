#include "StdAfx.h"
#include "HapticEvent.h"


HapticEvent::HapticEvent(::Effect effect, float duration, boost::uuids::uuid handle):Dirty(false), Effect(effect), Duration(duration), TimeElapsed(0), Sent(false),
Handle(handle)
{
}

HapticEvent::~HapticEvent()
{
}
bool HapticEvent::operator==(const HapticEvent& rhs) {
	return Handle == rhs.Handle;
}





::Duration HapticEvent::DurationType() const
{
	if (Duration > 0)
	{
		return ::Duration::Variable;
	}
	//cast to int
	return ::Duration(int(Duration));
}
