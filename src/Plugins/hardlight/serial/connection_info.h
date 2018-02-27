#pragma once
#include <boost/variant.hpp>
#include <string>

struct wired_connection {
	std::string port_name;
};

struct wifi_connection {
	std::string host_name;
	std::string port_number;
	std::string password;
};

using connection_info = boost::variant<wired_connection, wifi_connection>;

