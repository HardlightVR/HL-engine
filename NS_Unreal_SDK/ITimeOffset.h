#pragma once
class ITimeOffset
{
public:
	virtual ~ITimeOffset(void) {}
	virtual float GetTime() = 0;
};

