#include "stdafx.h"
#include "SerialPortIO.h"


SerialPortConnector::SerialPortConnector(boost::asio::serial_port &p, wired_connection args) 
	: port(p)
	, args(args)
{
}

void SerialPortConnector::try_connect(std::function<void(bool ec)> on_connect) {

	boost::system::error_code ec;
	port.open(args.port_name, ec);
	
	on_connect(port.is_open());
}

