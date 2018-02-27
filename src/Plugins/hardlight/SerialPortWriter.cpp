#include "stdafx.h"
#include "SerialPortWriter.h"

SerialPortWriter::SerialPortWriter(std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> outgoing, boost::asio::serial_port & port)
	: m_outgoing(outgoing)
	, m_port(port)
	, m_stopped(false)
	, m_totalBytes(0)
	, m_delay(boost::posix_time::millisec(5))
	, m_timer(port.get_io_service())
{
}



void SerialPortWriter::start()
{
	do_write();
}

void SerialPortWriter::stop()
{
	m_stopped = true;
}

std::size_t SerialPortWriter::total_bytes_written() const
{
	return m_totalBytes;
}

void SerialPortWriter::do_write()
{
	auto self(shared_from_this());

	auto available_bytes = m_outgoing->read_available();

	std::size_t amount_to_write = std::min<std::size_t>(m_tempBuffer.size(), available_bytes);

	assert(available_bytes % 16 == 0);


	const auto actual_popped = m_outgoing->pop(m_tempBuffer.data(), amount_to_write);
	assert(actual_popped <= m_tempBuffer.size());
	m_port.async_write_some(boost::asio::buffer(m_tempBuffer.data(), actual_popped), [this, self, actual_popped](auto ec, std::size_t bytes_transferred) {

		if (!ec) {
			/*if (actual_popped > 0) {
			std::stringstream ss;
			for (int i = 0; i < actual_popped; i++) {
			ss << std::to_string(m_tempBuffer[i]) << ", ";
			}
			std::cout << "wrote " << ss.str() << '\n';
			}*/
			m_totalBytes += bytes_transferred;

			m_timer.expires_from_now(m_delay);
			m_timer.async_wait([this, self](auto ec) { if (ec) { return; } if (m_stopped) { return; } do_write(); });
		}
	});
}
