#include "stdafx.h"
#include "Chip.h"
#include <chrono>
#include <iostream>
Chip::Chip() : m_clock_rate_mhz(16.0), m_running(true)
{
	
}


Chip::~Chip()
{
}

int Chip::Boot()
{
	using namespace std::chrono;
	double nano_period = 1000.0 / m_clock_rate_mhz;
	bool do_next_frame = true;
	double total_nanoseconds_elapsed = 0.0;
	auto then = steady_clock::now();

	if (m_firmware.Initialize() < 0) {
		return -1;
	}

	while (m_running) {
		
		if (do_next_frame) {
			int ret_code = m_firmware.Update();
			if (ret_code < 0) { return ret_code; }
			do_next_frame = false;
		}
		total_nanoseconds_elapsed += duration_cast<nanoseconds>(steady_clock::now() - then).count();
		then = steady_clock::now();

		if (total_nanoseconds_elapsed >= nano_period) {
			do_next_frame = true;
			total_nanoseconds_elapsed = 0.0;
		}
	}

	return 0;
}
