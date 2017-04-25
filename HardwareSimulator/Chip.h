#pragma once
#include <functional>
#include "Firmware.h"
class Chip
{
public:
	Chip();
	~Chip();
	int Boot();
private:
	double m_clock_rate_mhz;
	bool m_running;
	Firmware m_firmware;
};

