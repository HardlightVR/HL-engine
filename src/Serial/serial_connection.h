#pragma once

#include <array>
#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>

class device_profile;
class serial_connection_manager;
//precondition: port is open
class serial_connection : public std::enable_shared_from_this<serial_connection> {

public:
	serial_connection(boost::asio::serial_port port, std::string portName, device_profile* profile, serial_connection_manager& manager);
	void start();
	void stop();

private:
	bool m_connected;
	void do_read();
	void do_write();
	device_profile* m_profile;
	std::string m_portName;
	std::array<uint8_t, 128> m_buffer;
	std::array<uint8_t, 16> m_ping;
	boost::asio::serial_port m_port;

	serial_connection_manager& m_manager;
};

using serial_connection_ptr = std::shared_ptr<serial_connection>;