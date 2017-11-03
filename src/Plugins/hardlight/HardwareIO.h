#pragma once
#include <cstdint>
#include <memory>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/asio/serial_port.hpp>
#include "ReaderAdapter.h"
#include "WriterAdapter.h"

class HardwareIO {
public:
	HardwareIO(std::unique_ptr<boost::asio::serial_port> port);
	~HardwareIO() { std::cout << "destroying HWIo\n"; }
	boost::lockfree::spsc_queue<uint8_t>& incoming_queue();
	boost::lockfree::spsc_queue<uint8_t>& outgoing_queue();

	void start();
	void stop();

private:
	std::unique_ptr<boost::asio::serial_port> m_port;

	boost::lockfree::spsc_queue<uint8_t> m_incoming;
	boost::lockfree::spsc_queue<uint8_t> m_outgoing;
	std::shared_ptr<ReaderAdapter> m_reader;
	std::shared_ptr<WriterAdapter> m_writer;
};