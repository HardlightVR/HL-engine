#pragma once
class ITimeOffset
{
public:
	virtual ~ITimeOffset(void) {}
	virtual float Time() = 0;
};

