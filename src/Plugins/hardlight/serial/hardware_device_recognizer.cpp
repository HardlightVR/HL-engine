#include "stdafx.h"
#include "hardware_device_recognizer.h"
#include "enumser.h"
#include <boost/optional.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

#include "mark3.h"

#include "Doctor.h"

std::set<std::string> hardware_device_recognizer::get_interfaces() {
	
	CEnumerateSerial::CPortsArray ports;
	CEnumerateSerial::UsingQueryDosDevice(ports);
	
	std::set<std::string> portNames;
	for (unsigned int port : ports) {
		portNames.insert("COM" + std::to_string(port));
	}


	portNames.insert(m_permRecognizedInterfaces.begin(), m_permRecognizedInterfaces.end());

	return portNames;
}


//precondition: m_profiles is not empty
//Not meant to be used with no profiles
hardware_device_recognizer::hardware_device_recognizer(boost::asio::io_service& io, Doctor* doctor)
	: m_io(io)
	, m_doctor(doctor)
	, m_manager()
	, m_recognizedPorts()
	, m_scanTimeout(boost::posix_time::millisec(1000))
	, m_scanDeadline(m_io)
	, m_scanInterval(boost::posix_time::millisec(500))
	, m_scanIntervalTimer(m_io)
	, m_profiles()
	, m_currentProfile()
	, m_onRecognize()
	, m_onUnrecognize()
{
	m_profiles.push_back(std::make_unique<mark3>());
	
	m_currentProfile = m_profiles.begin();
	
	m_manager.on_connect([this](connection_info info) { this->handle_recognize(std::move(info)); });

}

void hardware_device_recognizer::start()
{
	do_port_scan();
}

void hardware_device_recognizer::stop()
{
	m_scanDeadline.cancel();
}

void hardware_device_recognizer::on_recognize(recognized_event handler)
{
	m_onRecognize = handler;
}

void hardware_device_recognizer::on_unrecognize(unrecognized_event handler)
{
	m_onUnrecognize =handler;
}

void hardware_device_recognizer::recognize(wifi_connection conn)
{
	
	m_permRecognizedInterfaces.insert(conn.host_name); //to keep it from being removed (unrecognized) immediately

	std::string pass = conn.password + (char)0x0d;
	m_onRecognize(wifi_connection{ conn.host_name, conn.port_number, pass });
}

size_t hardware_device_recognizer::get_num_potential_devices() const
{
	return m_manager.get_num_connections();
}


bool open_port(boost::asio::serial_port& port, const std::string& device_name) {
	boost::system::error_code ec;

	port.open(device_name, ec);
	if (ec) {
		core_log(nsvr_severity_error, "DeviceRecognizer", "Error opening" + device_name + ": " + ec.message());
	}
	else {
		core_log(nsvr_severity_trace, "DeviceRecognizer", "Port " + device_name + " was opened for writing");

	}
	return !ec;
}

void hardware_device_recognizer::remove_unrecognized_devices(const std::set<std::string>& newPorts) {

	std::vector<std::string> toBeRemoved;

	std::set_difference(m_recognizedPorts.begin(), m_recognizedPorts.end(), newPorts.begin(), newPorts.end(), std::back_inserter(toBeRemoved));

	for (auto portName : toBeRemoved) {
		
		m_recognizedPorts.erase(portName);
		m_onUnrecognize(portName);
	}
}

bool hardware_device_recognizer::device_already_recognized(const std::string& portName) const
{
	return m_recognizedPorts.find(portName) != m_recognizedPorts.end();
}


void hardware_device_recognizer::do_port_scan()
{
	std::set<std::string> interfaces = get_interfaces();

	remove_unrecognized_devices(interfaces);

	for (auto iface : interfaces) {

		if (device_already_recognized(iface)) {
			continue;
		}

		//ideally each interface name would have associated data, like if it is a port or a wifi connection. This way
		//I wouldn't be doing what is essentially a hack.. testing to see if it is probably an IP address.


		if (iface.find("192") == std::string::npos) {
			auto port = m_manager.make_port();
			if (open_port(*port, iface)) {
				(*m_currentProfile)->set_options(*port);
				m_manager.start(std::make_shared<serial_connection>(std::move(port), iface, (*m_currentProfile).get(), m_manager));
			}
		}
	}
	
	
	schedule_port_scan();
}


void hardware_device_recognizer::schedule_port_scan()
{
	m_scanDeadline.expires_from_now(m_scanTimeout);
	m_scanDeadline.async_wait([this](auto ec) {
		if (ec) { 
			return; 
		}

		//Special sauce: tell all the open or pending connections to end, and then destroy the io_service.
		//This is necessary because serial ports are tricky and there's weird cases where the entire io_service must be destroyed.

		m_manager.stop_all();
		m_manager.reset_io();

		//Loop to the next profile, if we have any
		m_currentProfile++;
		if (m_currentProfile == m_profiles.end()) {
			m_currentProfile = m_profiles.begin();
		}

		m_scanIntervalTimer.expires_from_now(m_scanInterval);
		m_scanIntervalTimer.async_wait([this](auto ec) { if (ec) { return; } do_port_scan();  });
	});
}



void hardware_device_recognizer::handle_recognize(connection_info info)
{
	m_recognizedPorts.insert(boost::apply_visitor(get_interface_name(), info));

	m_onRecognize(info);
}

