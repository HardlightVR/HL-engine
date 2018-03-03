#pragma once

#include "ReaderAdapter.h"

class SerialPortReader  {
public:
	SerialPortReader(boost::lockfree::spsc_queue<uint8_t>& incoming, boost::asio::serial_port& port);
	void start();
	void stop();

	std::size_t total_bytes_read() const;
private:
	boost::lockfree::spsc_queue<uint8_t>& m_incoming;
	constexpr static std::size_t INCOMING_DATA_BUFFER_SIZE = 128;
	std::array<uint8_t, INCOMING_DATA_BUFFER_SIZE> m_tempBuffer;
	boost::asio::serial_port& m_port;
	void do_read();
	bool m_stopped;
	std::size_t m_totalBytes;

};
