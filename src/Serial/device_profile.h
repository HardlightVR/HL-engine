#pragma once
#include <boost/asio/serial_port.hpp>
#include <vector>
class device_profile {
public:
	virtual void set_options(boost::asio::serial_port* port) const = 0;
	virtual const std::vector<boost::asio::const_buffer> get_ping_data() const = 0;

};