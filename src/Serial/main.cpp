#include "stdafx.h"
#include "hardware_device_recognizer.h"
#include <iostream>
#include <boost/asio/io_service.hpp>
int main() {

	boost::asio::io_service io;
	hardware_device_recognizer recog(io);


	recog.start();

	io.run();
	return 0;
}