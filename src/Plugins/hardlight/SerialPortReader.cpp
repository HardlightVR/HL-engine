#include "stdafx.h"
#include "SerialPortReader.h"

SerialPortReader::SerialPortReader(std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> incoming, boost::asio::serial_port & port)
	: m_incoming(incoming)
	, m_port(port)
	, m_stopped(false)
	, m_totalBytes(0)
{
}



void SerialPortReader::start()
{
	do_read();
}

void SerialPortReader::stop() {
	m_stopped = true;
}

std::size_t SerialPortReader::total_bytes_read() const
{
	return m_totalBytes;
}


void SerialPortReader::do_read()
{
	auto self(shared_from_this());

	m_port.async_read_some(boost::asio::buffer(m_tempBuffer, INCOMING_DATA_BUFFER_SIZE), [self, this](auto ec, std::size_t bytes_transferred) {
		if (m_stopped) {
			return;
		}
		if (!ec) {
			m_incoming->push(m_tempBuffer.data(), bytes_transferred);
			m_totalBytes += bytes_transferred;
			do_read();
		}
	});
}
