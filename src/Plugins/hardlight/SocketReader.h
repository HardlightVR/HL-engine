#pragma once

#include "ReaderAdapter.h"
#include <boost/asio/ip/tcp.hpp>

const int max_read_length = 256;

class SocketReader {
public:
	SocketReader(boost::lockfree::spsc_queue<uint8_t>& incoming, boost::asio::ip::tcp::socket& socket);
	// Inherited via ReaderAdapter
	void start() ;
	void stop();
	std::size_t total_bytes_read() const ;

private:
	void read_start(void);
	void read_complete(const boost::system::error_code& error, size_t bytes_transferred);

	boost::asio::ip::tcp::socket& m_socket;
	boost::lockfree::spsc_queue<uint8_t>& m_incoming;
	uint8_t m_readMsg[max_read_length];
};