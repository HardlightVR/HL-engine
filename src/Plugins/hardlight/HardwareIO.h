#pragma once
#include <cstdint>
#include <memory>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/asio/serial_port.hpp>
#include "ReaderAdapter.h"
#include "WriterAdapter.h"


class HardwareIO {
public:
	HardwareIO();
	std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> incoming_queue();
	std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> outgoing_queue();

	void start();
	void stop();

	std::size_t outgoing_queue_capacity() const;
	std::size_t outgoing_queue_size() const;
	std::size_t bytes_read() const;
	std::size_t bytes_written() const;

private:
	virtual void do_interface_cleanup() = 0;
	virtual void do_interface_creation() = 0;
	virtual ReaderAdapter* get_reader() const = 0;
	virtual WriterAdapter* get_writer() const = 0;



	std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> m_incoming;
	std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> m_outgoing;
	
};