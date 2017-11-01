#include "stdafx.h"
#include <memory>

#include "serial_connection.h"
#include "serial_connection_manager.h"
#include <iostream>
#include "device_profile.h"
#include <sstream>
serial_connection::serial_connection(boost::asio::serial_port port, std::string portName, device_profile* profile, serial_connection_manager& manager)
	: m_port(std::move(port))
	, m_connected(false)
	, m_profile(profile)
	, m_portName(portName)
	, m_manager(manager)
	, m_buffer()
	, m_ping({0x24, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x0D, 0x0A })
{
}

void serial_connection::start()
{
	do_write();
}

void serial_connection::stop()
{
	boost::system::error_code ignored;
	m_port.close(ignored);

	if (ignored) {
		std::cout << "Fatal error trying to close  " << m_portName << ": " << ignored.message() << '\n';

	}
	if (m_port.is_open()) {
		std::cout << "Fatal error on " << m_portName << ": the port is still open after closing\n";
	}
}

void serial_connection::do_read()
{
	std::fill(m_buffer.begin(), m_buffer.end(), 0);

	auto self(shared_from_this());
	m_port.async_read_some(boost::asio::buffer(m_buffer), [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {

		if (ec == boost::asio::error::operation_aborted) {
			std::cout << "Write operation on " << m_portName << " aborted due to timeout\n";

		}

		if (!ec) {
			//stand-in for an actual packet parser
			if (bytes_transferred == 16) {
				std::cout << "Got a packet response on " << m_portName << "\n";
			}
			else {
				std::cout << "Got an unknown packet response [" << bytes_transferred << " bytes] on " << m_portName << "\n";
				std::stringstream ss;
				for (int i = 0;i < 128; i++) {
					ss << std::to_string(m_buffer[i]) << ", ";

				}
				std::cout << "	" << ss.str() << '\n';


			}
			m_manager.stop(shared_from_this());

			
		}
		else if (ec != boost::asio::error::operation_aborted) {
			std::cout << "Read operation error on " << m_portName << ": " << ec.message() << "\n";
			m_manager.stop(shared_from_this());
		}
	});
}

void serial_connection::do_write()
{
	auto self(shared_from_this());

	m_port.async_write_some(m_profile->get_ping_data(), [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
		//if there's no errors, proceed to reading from the port
		if (!ec) {
			do_read();
			return;
		}

		if (ec == boost::asio::error::operation_aborted) {
			std::cout << "Write operation on " << m_portName << " aborted due to timeout\n";

		}

		//Else as long as we weren't aborted (due to e.g. someone calling stop) but there was some kind of error,
		//we should tell the manager to stop us
		if (ec != boost::asio::error::operation_aborted) {
			std::cout << "Write operation error on " << m_portName << ": " << ec.message() << "\n";

			m_manager.stop(shared_from_this());
		}
		
	
	});
}
