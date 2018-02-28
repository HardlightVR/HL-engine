#pragma once
#include <boost/asio/serial_port.hpp>
#include "serial/connection_info.h"
class SerialPortConnector {

public:
	SerialPortConnector(boost::asio::serial_port&, wired_connection args);
	void try_connect(std::function<void(bool ec)> on_connect);
private:
	boost::asio::serial_port& port;
	wired_connection args;
};