#pragma once
#include "ITimeOffset.h"
#include <memory>

class TimeInstant
{
public:
	float Time;
	std::unique_ptr<ITimeOffset> Item;
	bool Expired() const;
	TimeInstant(float t, std::unique_ptr<ITimeOffset> to);
	~TimeInstant();
	friend void swap(TimeInstant& first, TimeInstant& second)
	{
		using std::swap;
		swap(first.Time, second.Time);
		swap(first.Item, second.Item);
	}
	TimeInstant(TimeInstant &&bar);

	TimeInstant& operator=(TimeInstant&& bar);
};

