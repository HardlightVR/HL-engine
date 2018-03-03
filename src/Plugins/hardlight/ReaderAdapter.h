#pragma once

#include <memory>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/asio/serial_port.hpp>


class ReaderAdapter {
public:
	virtual void start() = 0;
	virtual void stop() = 0;

	virtual std::size_t total_bytes_read() const= 0;


};
