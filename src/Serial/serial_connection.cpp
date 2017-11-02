#include "stdafx.h"
#include <memory>

#include "serial_connection.h"
#include "serial_connection_manager.h"
#include <iostream>
#include "device_profile.h"
#include <sstream>
serial_connection::serial_connection(boost::asio::serial_port port, std::string portName, const device_profile* profile, serial_connection_manager& manager)
	: m_manager(manager)
	, m_buffer()
	, m_port(std::move(port))
	, m_portName(portName)
	, m_profile(profile)
	, m_connectionInfo()
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

boost::optional<connection_info> serial_connection::get_detected_info() const
{
	return m_connectionInfo;
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

				m_connectionInfo = connection_info{m_portName,  m_buffer[3], m_buffer[4], m_buffer[5], m_buffer[6] };
				
			}
			else {
				//todo: log (there is tracking data being returned, firmware must be fixed for this)
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

	
		if (ec != boost::asio::error::operation_aborted) {
			m_manager.stop(shared_from_this());
		}
	
	});
}
