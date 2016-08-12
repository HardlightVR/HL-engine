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
};

