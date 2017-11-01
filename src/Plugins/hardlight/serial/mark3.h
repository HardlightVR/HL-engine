#pragma once

#include <boost/asio/serial_port.hpp>
#include <array>

#include "device_profile.h"

class mark3 : public device_profile {
public:
	mark3();
	void set_options(boost::asio::serial_port* port) const override;
private:
	const boost::asio::const_buffer do_get_ping_data() const override;
	const std::array<uint8_t, 16> m_pingData;
};

