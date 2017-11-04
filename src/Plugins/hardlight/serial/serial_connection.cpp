#include "stdafx.h"
#include <memory>

#include "serial_connection.h"
#include "serial_connection_manager.h"
#include <iostream>
#include "device_profile.h"
#include <sstream>
#include "logger.h"
serial_connection::serial_connection(std::unique_ptr<boost::asio::serial_port> port, std::string portName, const device_profile* profile, serial_connection_manager& manager)
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
	m_port->close(ignored);

	if (ignored) {
		core_log(nsvr_severity_error, "SerialConnection", "Fatal error trying to close  " + m_portName + ": " + ignored.message());

	}
	if (m_port->is_open()) {
		core_log(nsvr_severity_error, "SerialConnection", "Port " + m_portName + " is still open after closing?");
	}
}

std::unique_ptr<boost::asio::serial_port> serial_connection::steal_port()
{
	return std::move(m_port);
}

std::string serial_connection::port_name() const
{
	return m_portName;
}


void serial_connection::do_read()
{
	std::fill(m_buffer.begin(), m_buffer.end(), 0);

	auto self(shared_from_this());
	m_port->async_read_some(boost::asio::buffer(m_buffer), [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {

		if (ec == boost::asio::error::operation_aborted) {
			core_log(nsvr_severity_trace, "SerialConnection", "Read operation on " + m_portName + " aborted due to timeout");

		}

		if (!ec) {

			
			if (m_profile->is_valid_response(m_buffer, bytes_transferred)) {
				m_manager.raise_connect(shared_from_this());
			}
			
			//stand-in for an actual packet parser
			else {
				//todo: log (there is tracking data being returned, firmware must be fixed for this)
				core_log(nsvr_severity_trace, "SerialConnection", "Got an unknown packet response [" + std::to_string(bytes_transferred) + " bytes] on " + m_portName);
				std::stringstream ss;
				for (int i = 0;i < 128; i++) {
					ss << std::to_string(m_buffer[i]) << ", ";

				}
			
				core_log(nsvr_severity_trace, "SerialConnection", ss.str());
				m_manager.stop(shared_from_this());

			}



			
		}
		else if (ec != boost::asio::error::operation_aborted) {
			m_manager.stop(shared_from_this());
		}
	});
}

void serial_connection::do_write()
{
	auto self(shared_from_this());

	m_port->async_write_some(m_profile->get_ping_data(), [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
		//if there's no errors, proceed to reading from the port
		if (!ec) {
			do_read();
			return;
		}

		if (ec == boost::asio::error::operation_aborted) {
			core_log(nsvr_severity_trace, "SerialConnection", "Write operation on " + m_portName + " aborted due to timeout");
		}
	
		if (ec != boost::asio::error::operation_aborted) {
			m_manager.stop(shared_from_this());
		}
	
	});
}
