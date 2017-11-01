#include "stdafx.h"
#include "hardware_device_recognizer.h"
#include <iostream>
#include <boost/asio/io_service.hpp>
#include "connection_info.h"
int main() {

	boost::asio::io_service io;
	hardware_device_recognizer recog(io);

	recog.on_recognize([](connection_info info) {
		std::cout << "Got a new device on " << info.port << ": mark " << std::to_string(info.product) << '\n';
	});

	recog.on_unrecognize([](std::string portName) {
		std::cout << "The device on " << portName << " disappeared\n";
	});

	recog.start();

	io.run();
	return 0;
}