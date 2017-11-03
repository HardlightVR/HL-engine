#pragma once


#include <memory>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/asio/serial_port.hpp>

class WriterAdapter : public std::enable_shared_from_this<WriterAdapter> {
public:
	WriterAdapter(std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> outgoing, boost::asio::serial_port& port);
	void start();
	void stop();

	std::size_t total_bytes_written() const;
private:
	bool m_stopped;
	std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> m_outgoing;
	std::array<uint8_t, 64> m_tempBuffer;

	boost::asio::serial_port& m_port;

	void do_write();

	std::size_t m_totalBytes;

};
