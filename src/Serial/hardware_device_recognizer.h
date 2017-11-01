#pragma once
#include <memory>

#include "serial_connection_manager.h"
#include <boost/asio/deadline_timer.hpp>
#include "device_profile.h"

class mark3 : public device_profile {
public:
	mark3();
	void set_options(boost::asio::serial_port* port) const override;
private:
	const boost::asio::const_buffer do_get_ping_data() const override;
	const std::array<uint8_t, 16> m_pingData;
};

class mark2 : public device_profile {
public:
	mark2();
	void set_options(boost::asio::serial_port* port) const override;
private:
	const boost::asio::const_buffer do_get_ping_data() const override;
	const std::array<uint8_t, 7> m_pingData;
};


class hardware_device_recognizer {
public:
	hardware_device_recognizer(boost::asio::io_service& io);

	void start();
	void stop();
private:
	void do_port_scan();

	boost::asio::io_service& m_io;
	serial_connection_manager m_manager;
	std::set<std::string> m_activePortSet;

	boost::posix_time::millisec m_scanTimeout;
	boost::asio::deadline_timer m_scanDeadline;

	std::vector<std::unique_ptr<device_profile>> m_profiles;
	decltype(m_profiles)::iterator m_currentProfile;


};