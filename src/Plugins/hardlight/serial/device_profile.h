#pragma once
#include <boost/asio/serial_port.hpp>
#include <vector>


//Could potentially have derived classes construct device_profile with the ping data.. or not
class device_profile {
public:
	virtual void set_options(boost::asio::serial_port* port) const = 0;
	const std::vector<boost::asio::const_buffer> get_ping_data() const;
	virtual bool is_valid_response(const std::array<uint8_t, 128>& buffer, std::size_t length) const = 0;
private:
	virtual const boost::asio::const_buffer do_get_ping_data() const = 0;
};