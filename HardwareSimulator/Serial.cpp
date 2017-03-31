#include "stdafx.h"
#include "Serial.h"
#include <iostream>

Serial::Serial(boost::asio::io_service& io) : m_port(io)
{
}


Serial::~Serial()
{
}

bool Serial::Connect(std::string portName)
{
	try {
		m_port.open(portName);
		if (!m_port.is_open()) {
			return false;
		}
	}
	catch (const boost::system::system_error& ec) {
		return false;
	}

	return true;
}

void Serial::Write(std::shared_ptr<uint8_t*> bytes, std::size_t length)
{
	if (m_port.is_open()) {
		m_port.async_write_some(boost::asio::buffer(*bytes, length),
			[bytes](const boost::system::error_code& error, std::size_t bytes_transferred) {
			if (error) {
				std::cout << "Error writing to port\n";
			}
		});
	}
}
