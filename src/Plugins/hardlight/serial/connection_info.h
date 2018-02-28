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

class get_interface_name : public boost::static_visitor<std::string> {
public:
	inline std::string operator()(wired_connection conn) const
	{
		return conn.port_name;
	}
	inline std::string operator()(wifi_connection conn) const {
		return conn.host_name + conn.port_number;
	}
};