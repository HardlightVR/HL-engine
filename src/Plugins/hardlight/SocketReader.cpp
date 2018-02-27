#include "stdafx.h"
#include "SocketReader.h"

SocketReader::SocketReader(std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> incoming, boost::asio::ip::tcp::socket & socket)
	: m_incoming(incoming)
	, m_socket(socket)
{
}

void SocketReader::start()
{
	read_start();
}

void SocketReader::stop()
{
}

std::size_t SocketReader::total_bytes_read() const
{
	return std::size_t();
}

void SocketReader::read_start(void)
{

	auto self(shared_from_this());

	m_socket.async_read_some(boost::asio::buffer(m_readMsg, max_read_length), [self, this](const boost::system::error_code& ec, std::size_t bt) {
		read_complete(ec, bt);
	});
}


void SocketReader::read_complete(const boost::system::error_code & error, size_t bytes_transferred)
{
	if (!error)
	{ // read completed, so process the data
	  

		m_incoming->push(m_readMsg, bytes_transferred);
		std::fill(std::begin(m_readMsg), std::end(m_readMsg), 0);


		//cout << "\n";
		read_start(); // start waiting for another asynchronous read again
	}
	
}
