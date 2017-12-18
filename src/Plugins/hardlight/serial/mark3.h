#pragma once

#include <boost/asio/serial_port.hpp>
#include <array>

#include "device_profile.h"


//I guess fundamentally, it's not a mark3 profile, but a mark3-compatible instruction set..
//Because we are not detecing that a "mark3" is connected, simply that the device responds to these mark3 commands. 
class mark3 : public device_profile {
public:
	mark3();
	void set_options(boost::asio::serial_port& port) const override;
	bool is_valid_response(const std::array<uint8_t, 128>& buffer, std::size_t length) const override;
private:
	const boost::asio::const_buffer do_get_ping_data() const override;
	const std::array<uint8_t, 16> m_pingData;
};

