#pragma once


#include <boost/asio/ip/tcp.hpp>
#include "serial/connection_info.h"
class WifiConnector {
public:
	using tcp = boost::asio::ip::tcp;
	using on_connect = std::function<void(bool success)>;
	WifiConnector(tcp::socket&, wifi_connection args);
	void try_connect(on_connect handler);
private:
	void connect_start();
	void connect_complete(const boost::system::error_code& error);
	void read_start();
	void read_complete(const boost::system::error_code& ec, size_t bytes_transferred);
	void send_password();

	tcp::socket& socket;
	wifi_connection args;
	tcp::resolver::iterator m_endpoint;

	on_connect connect;

	char read_msg[256];
};