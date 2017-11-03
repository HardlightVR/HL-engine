#pragma once

#include <memory>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/asio/serial_port.hpp>

class ReaderAdapter : public std::enable_shared_from_this<ReaderAdapter>{
public:
	ReaderAdapter(boost::lockfree::spsc_queue<uint8_t>& incoming, boost::asio::serial_port& port);
	~ReaderAdapter() { std::cout << "Destroying readeradapter\n"; }
	void start();

private:
	boost::lockfree::spsc_queue<uint8_t>& m_incoming;
	constexpr static std::size_t INCOMING_DATA_BUFFER_SIZE = 128;
	std::array<uint8_t, INCOMING_DATA_BUFFER_SIZE> m_tempBuffer;
	boost::asio::serial_port& m_port;
	void do_read();

};