#pragma once
#include <vector>
#include <memory>
#include <set>

#include <boost/asio/io_service.hpp>
#include <boost/signals2.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "connection_info.h"
#include "serial_connection_manager.h"
#include "device_profile.h"

#include "logger.h"
class hardware_device_recognizer {
public:
	using recognized_event = boost::signals2::signal<void(connection_info)>;
	using unrecognized_event = boost::signals2::signal<void(connection_info)>;

	hardware_device_recognizer(boost::asio::io_service& io);
	
	void start();
	void stop();

	void on_recognize(recognized_event::slot_type handler);
	void on_unrecognize(unrecognized_event::slot_type handler);

private:
	void do_port_scan();
	void handle_recognize(connection_info info);
	void remove_unrecognized_devices(const std::set<std::string>& newPorts);
	bool device_already_recognized(const std::string& portName) const;
	void schedule_port_scan();

	boost::asio::io_service& m_io;
	
	serial_connection_manager m_manager;
	std::set<std::string> m_recognizedPorts;

	boost::posix_time::millisec m_scanTimeout;
	boost::asio::deadline_timer m_scanDeadline;

	boost::posix_time::millisec m_scanInterval;
	boost::asio::deadline_timer m_scanIntervalTimer;

	std::vector<std::unique_ptr<device_profile>> m_profiles;
	decltype(m_profiles)::iterator m_currentProfile;

	recognized_event m_onRecognize;
	unrecognized_event m_onUnrecognize;

};