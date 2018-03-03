#pragma once

#include "ReaderAdapter.h"
#include <boost/asio/ip/tcp.hpp>
const int max_read_length = 256;

template<typename IoObject, size_t packet_size>
class Reader {
public:
	Reader(boost::lockfree::spsc_queue<uint8_t>& incoming, IoObject& socket)
		: m_incoming(incoming)
		, m_socket(socket) {}

	void start() {
		read_start();
	}

	std::size_t total_bytes_read() const {
		return std::size_t();
	}

	void read_start(void) {
		m_socket.async_read_some(boost::asio::buffer(m_readMsg, max_read_length), [this](const boost::system::error_code& ec, std::size_t bt) {
			read_complete(ec, bt);
		});
	}


	void read_complete(const boost::system::error_code & error, size_t bytes_transferred) {
		if (!error) {
			if (m_incoming.write_available() >= bytes_transferred) {
				m_incoming.push(m_readMsg, bytes_transferred);
				std::fill(std::begin(m_readMsg), std::end(m_readMsg), 0);
			}
			read_start(); // start waiting for another asynchronous read again
		}
	}


private:
	IoObject& m_socket;
	boost::lockfree::spsc_queue<uint8_t>& m_incoming;
	uint8_t m_readMsg[packet_size];
};