#pragma once
#include "Serial.h"
#include <boost\asio\io_service.hpp>
class Firmware
{
public:
	Firmware();
	~Firmware();
	int Initialize();
	int Update();
private:
	boost::asio::io_service m_io;
	Serial m_serial;
};

