#include "stdafx.h"
#include "WifiConnector.h"
#include <boost/asio.hpp>

WifiConnector::WifiConnector(tcp::socket & socket, wifi_connection args)
	: socket(socket)
	, args(args)
{
	tcp::resolver resolver(socket.get_io_service());
	tcp::resolver::query query(args.host_name, args.port_number);
	m_endpoint = resolver.resolve(query);


	std::fill(std::begin(read_msg), std::end(read_msg), 0);
}

void WifiConnector::try_connect(on_connect handler)
{
	connect = handler;
	connect_start(); 

}



void WifiConnector::connect_start()
{
	tcp::endpoint endpoint = *m_endpoint;
	m_endpoint++;

	socket.async_connect(endpoint, [this](const boost::system::error_code& ec) {
		connect_complete(ec);
	});
}

void WifiConnector::connect_complete(const boost::system::error_code & error)
{
	if (!error) {
		socket.set_option(tcp::no_delay(true));
		read_start();
	}
	else if (m_endpoint != tcp::resolver::iterator()) {
		socket.close();
		connect_start();
	}

}

void WifiConnector::read_start()
{
	socket.async_read_some(boost::asio::buffer(read_msg, 255), [this](const auto& ec, size_t bt) {
		read_complete(ec, bt);
	});
}

void WifiConnector::read_complete(const boost::system::error_code & ec, size_t bytes_transferred)
{
	if (!ec) {
		std::string msg = read_msg;
		if (msg.find("Password:") != std::string::npos) {
			send_password();
		}
		else {
			std::fill(std::begin(read_msg), std::end(read_msg), 0);
			read_start();
		}

	}
}

void WifiConnector::send_password()
{
	boost::asio::async_write(socket, boost::asio::buffer(args.password), [this](const auto& ec, size_t bt) {
		connect(!ec);
	});
}
