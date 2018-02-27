#pragma once

#include <memory>
#include <boost/lockfree/spsc_queue.hpp>
#include <cstdint>

class IOInterface {
public:
	virtual void start() = 0;
	virtual void stop() = 0;

	virtual std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> incoming_queue() = 0;
	virtual std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> outgoing_queue() = 0;

	virtual std::size_t outgoing_queue_capacity() const = 0;
	virtual std::size_t outgoing_queue_size() const = 0;
	virtual std::size_t bytes_read() const = 0;
	virtual std::size_t bytes_written() const = 0;
};