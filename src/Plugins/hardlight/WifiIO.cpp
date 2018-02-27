#include "stdafx.h"
#include "WifiIO.h"
#include "SocketReader.h"
WifiIO::WifiIO(std::unique_ptr<boost::asio::ip::tcp::socket> socket, boost::asio::ip::tcp::resolver::iterator it, std::string pass)
	: HardwareIO()
	, m_endpoint(it)
	, m_password(pass)
	, m_socket(std::move(socket))
{

}

void WifiIO::do_interface_cleanup()
{
	m_reader->stop();
	m_writer->stop();

	m_socket->close();
}

ReaderAdapter * WifiIO::get_reader() const
{
	return m_reader.get();
}

WriterAdapter * WifiIO::get_writer() const
{
	return m_writer.get();
}

void WifiIO::do_interface_creation()
{
	connect_start(m_endpoint);
}


void WifiIO::connect_start(boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	using tcp = boost::asio::ip::tcp;

	tcp::endpoint endpoint = *m_endpoint;

	m_socket->async_connect(endpoint, [this, it = ++m_endpoint](const boost::system::error_code& ec) {
		connect_complete(ec, it);
	});
}

void WifiIO::connect_complete(const boost::system::error_code & error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	using tcp = boost::asio::ip::tcp;

	m_socket->set_option(tcp::no_delay(true));

	if (!error) {// success, so start waiting for read data
		//read_start
		m_reader = std::make_shared<SocketReader>(incoming_queue(), *m_socket);
		m_reader->start();
		//	m_writer = std::make_shared<SocketWriter>(outgoing_queue(), *m_socket);
	}
	else if (endpoint_iterator != tcp::resolver::iterator())
	{ // failed, so wait for another connection event
		m_socket->close();
		connect_start(endpoint_iterator);
	}
}

