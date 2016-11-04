#include "TimeInstant.h"


bool TimeInstant::Expired() const
{
	return Time >= Item->GetTime();
}

TimeInstant::TimeInstant(float t, std::unique_ptr<ITimeOffset> to):Time(t), Item(std::move(to))
{
}


TimeInstant::~TimeInstant()
{
}


TimeInstant::TimeInstant(TimeInstant&& bar): Time(bar.Time),  Item(std::move(bar.Item)) 
{

}

TimeInstant& TimeInstant::operator=(TimeInstant&& bar)
{
	Item = std::move(bar.Item); 
	return *this;
}
