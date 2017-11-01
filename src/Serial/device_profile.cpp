#include "stdafx.h"
#include "device_profile.h"

const std::vector<boost::asio::const_buffer> device_profile::get_ping_data() const
{
	return std::vector<boost::asio::const_buffer>{do_get_ping_data()};
}
