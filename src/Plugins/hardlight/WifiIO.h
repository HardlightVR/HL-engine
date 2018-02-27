#pragma once


#include "HardwareIO.h"
#include <boost/asio/ip/tcp.hpp>

class WifiIO : public HardwareIO {
public:
	WifiIO(std::unique_ptr<boost::asio::ip::tcp::socket> socket, boost::asio::ip::tcp::resolver::iterator it, std::string password);
private:
	void do_interface_cleanup() override;


	ReaderAdapter * get_reader() const override;
	WriterAdapter * get_writer() const override;

	std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;

	std::shared_ptr<ReaderAdapter> m_reader;
	std::shared_ptr<WriterAdapter> m_writer;


	// Inherited via HardwareIO
	virtual void do_interface_creation() override;

	void connect_start(boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
	void connect_complete(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
	

	std::string m_password;
	boost::asio::ip::tcp::resolver::iterator m_endpoint;
};