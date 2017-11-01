#pragma once
#include <memory>

#include "serial_connection_manager.h"
#include <boost/asio/deadline_timer.hpp>
#include "device_profile.h"

class mark3 : public device_profile {
public:
	mark3();
	void set_options(boost::asio::serial_port* port) const override;
	const std::vector<boost::asio::const_buffer>get_ping_data() const override;
private:
	const std::array<uint8_t, 16> m_pingData;
};
class hardware_device_recognizer {
public:
	hardware_device_recognizer(boost::asio::io_service& io);

	void scan_once();
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