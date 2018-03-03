#pragma once
#include <array>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/asio/serial_port.hpp>

constexpr int tcp_payload_size = 16;
class SocketWriter {
public:
	SocketWriter(boost::lockfree::spsc_queue<uint8_t>& outgoing, boost::asio::serial_port& port);
	void start();
	void stop();

	std::size_t total_bytes_written() const;
private:
	bool m_stopped;
	boost::lockfree::spsc_queue<uint8_t>& m_outgoing;
	std::array<uint8_t, tcp_payload_size> m_tempBuffer;

	boost::asio::serial_port& m_port;
	boost::posix_time::millisec m_delay;
	boost::asio::deadline_timer m_timer;
	void do_write();

	std::size_t m_totalBytes;

};