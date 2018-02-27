#pragma once

#include "ReaderAdapter.h"
#include <boost/asio/ip/tcp.hpp>

const int max_read_length = 256;

class SocketReader : public ReaderAdapter, public std::enable_shared_from_this<SocketReader> {
public:
	SocketReader(std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> incoming, boost::asio::ip::tcp::socket& socket);
	// Inherited via ReaderAdapter
	void start() override;
	void stop() override;
	std::size_t total_bytes_read() const override;

private:
	void read_start(void);
	void read_complete(const boost::system::error_code& error, size_t bytes_transferred);

	boost::asio::ip::tcp::socket& m_socket;
	std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> m_incoming;
	uint8_t m_readMsg[max_read_length];
};