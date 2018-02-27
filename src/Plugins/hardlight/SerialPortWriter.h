#pragma once
#include "WriterAdapter.h"
#include <boost/lockfree/spsc_queue.hpp>
//Increase to 128 when Mark2 is discontinued and not supported
constexpr std::size_t serial_buffer_size = 64;



class SerialPortWriter : public WriterAdapter, public std::enable_shared_from_this<SerialPortWriter> {
public:
	SerialPortWriter(std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> outgoing, boost::asio::serial_port& port);
	void start() override;
	void stop() override;

	std::size_t total_bytes_written() const override;
private:
	bool m_stopped;
	std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> m_outgoing;
	std::array<uint8_t, serial_buffer_size> m_tempBuffer;

	boost::asio::serial_port& m_port;
	boost::posix_time::millisec m_delay;
	boost::asio::deadline_timer m_timer;
	void do_write();

	std::size_t m_totalBytes;

};