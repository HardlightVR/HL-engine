#pragma once
#include <set>
#include "serial_connection.h"
#include <thread>
#include <boost/optional.hpp>
#include <boost/asio/io_service.hpp>
class serial_connection_manager {
public:
	serial_connection_manager();
	void start(serial_connection_ptr s);
	void stop(serial_connection_ptr s);
	void reset_io();
	void stop_all();


	boost::asio::serial_port make_port();
private:
	std::thread m_thread;
	std::unique_ptr<boost::asio::io_service> m_io;
	std::unique_ptr<boost::asio::io_service::work> m_work;
	std::set<serial_connection_ptr> m_connections;

};