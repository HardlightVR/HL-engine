#include "TimeInstant.h"


bool TimeInstant::Expired() const
{
	return Time > Item->Time();
}

TimeInstant::TimeInstant(float t, std::unique_ptr<ITimeOffset> to):Time(t), Item(std::move(to))
{
}


TimeInstant::~TimeInstant()
{
}
