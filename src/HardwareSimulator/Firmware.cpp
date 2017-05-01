#include "stdafx.h"
#include "Firmware.h"

#include <iostream>
#include <memory>
Firmware::Firmware() : m_serial(m_io)
{
}


Firmware::~Firmware()
{
}

int Firmware::Initialize()
{
	int retval = m_serial.Connect("COM6");

	return retval;
}

int Firmware::Update()
{

	auto pingData = std::make_shared<uint8_t*>(new uint8_t[7]{ '$', 0x02, 0x02, 0x07, 0xFF, 0xFF, 0x0A });
	m_serial.Write(pingData, 7);
	m_io.poll();
	return 1;
}
