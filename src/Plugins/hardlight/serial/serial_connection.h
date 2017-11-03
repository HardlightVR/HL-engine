#pragma once

#include <string>
#include <array>
#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/optional.hpp>

#include "connection_info.h"

class device_profile;
class serial_connection_manager;

//precondition: port is open
class serial_connection : public std::enable_shared_from_this<serial_connection> {

public:
	serial_connection(std::unique_ptr<boost::asio::serial_port> port, std::string portName, const device_profile* profile, serial_connection_manager& manager);
	
	void start();
	void stop();

	std::unique_ptr<boost::asio::serial_port>  steal_port();

	std::string port_name() const;
private:
	void do_read();
	void do_write();

	serial_connection_manager& m_manager;
	std::array<uint8_t, 128> m_buffer;
	std::unique_ptr<boost::asio::serial_port> m_port;
	std::string m_portName;
	const device_profile* m_profile;
	boost::optional<connection_info> m_connectionInfo;
};

using serial_connection_ptr = std::shared_ptr<serial_connection>;