#include "stdafx.h"
#include "hardware_device_recognizer.h"
#include "enumser.h"
#include <boost/optional.hpp>
#include <vector>
#include <iostream>
std::vector<std::string> getPortNames() {
	CEnumerateSerial::CPortsArray ports;
	CEnumerateSerial::UsingQueryDosDevice(ports);
	
	std::vector<std::string> portNames = {};
	for (unsigned int port : ports) {
		portNames.push_back("COM" + std::to_string(port));
	}
	return portNames;
}
//precondition: m_profiles is not empty
hardware_device_recognizer::hardware_device_recognizer(boost::asio::io_service& io)
	: m_io(io)
	, m_profiles()
	, m_scanTimeout(boost::posix_time::millisec(1000))
	, m_scanDeadline(m_io)
{
	m_profiles.push_back(std::make_unique<mark3>());
	assert(!m_profiles.empty());
	m_currentProfile = m_profiles.begin();
}

void hardware_device_recognizer::scan_once()
{
	do_port_scan();
}

/*

A portscan happens over the "inactive set" of ports
These are all the ports found that do not currently have a device attached to them 
whereas the active set has a device attached to it. 

Then portscans proceed like so:

Do portscan for hardlight mk1
<reset io service>
Do portscan for hardlight mk2
<reset io service>

Do portscan for hardlight mk1
<reset io service>

Do portscan for hardlight mk2


1. Retrieve the set of available ports on the system
2. for each name, if a port can be created with that name, initiate a connection 
3. Wait
4. If the 
*/

bool open_port(boost::asio::serial_port* port, const std::string& device_name) {
	boost::system::error_code ec;

	//note: this can take a second or so! This is synchronous!
	port->open(device_name, ec);
	if (ec) {
		std::cout << "Error opening " << device_name << ": " << ec.message() << '\n';
	}
	return !ec;
}
void hardware_device_recognizer::do_port_scan()
{
	std::vector<std::string> portNames = getPortNames();
	std::cout << "There are " << portNames.size() << " ports available\n";
	for (auto portName : portNames) {
		auto port = m_manager.make_port();
		std::cout << "Opening port " << portName << '\n';
		if (open_port(&port, portName)) {
			std::cout << "	" << portName << " opened.\n";
			(*m_currentProfile)->set_options(&port);

			m_manager.start(std::make_shared<serial_connection>(std::move(port), portName, (*m_currentProfile).get(), m_manager));

		}
	}

	m_scanDeadline.expires_from_now(m_scanTimeout);
	m_scanDeadline.async_wait([this](auto ec) { 
		if (ec) { return; } 
		m_manager.stop_all();
		m_manager.reset_io();

		m_currentProfile++;
		if (m_currentProfile == m_profiles.end()) {
			m_currentProfile = m_profiles.begin();
		}
		do_port_scan(); });
}

mark3::mark3() : m_pingData({ 0x24, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x0D, 0x0A })
{
}

void mark3::set_options(boost::asio::serial_port * port) const
{
	port->set_option(boost::asio::serial_port::baud_rate(115200));
	port->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
	port->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::hardware));
	port->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
	port->set_option(boost::asio::serial_port::character_size(8));
}

const std::vector<boost::asio::const_buffer> mark3::get_ping_data() const
{
	return std::vector<boost::asio::const_buffer>{boost::asio::const_buffer(m_pingData.data(), m_pingData.size())};
}
