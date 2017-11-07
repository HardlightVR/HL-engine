#include "stdafx.h"
#include "WriterAdapter.h"

WriterAdapter::WriterAdapter(std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> outgoing, boost::asio::serial_port & port)
	: m_outgoing(outgoing)
	, m_port(port)
	, m_stopped(false)
	, m_totalBytes(0)
{
}

void WriterAdapter::start()
{
	do_write();
}

void WriterAdapter::stop()
{
	m_stopped = true;
}

std::size_t WriterAdapter::total_bytes_written() const
{
	return m_totalBytes;
}

void WriterAdapter::do_write()
{
	auto self(shared_from_this());
	
	auto available_bytes = m_outgoing->read_available();

	std::size_t amount_to_write = std::min<std::size_t>(64, available_bytes);

	assert(available_bytes % 16 == 0);


	const int actual_popped = m_outgoing->pop(m_tempBuffer.data(), amount_to_write);
	assert(actual_popped <= 64);
	m_port.async_write_some(boost::asio::buffer(m_tempBuffer.data(),actual_popped), [this, self, actual_popped](auto ec, std::size_t bytes_transferred) {
		if (m_stopped) {
			return;
		}
		if (!ec) {
			if (actual_popped > 0) {
				std::stringstream ss;
				for (int i = 0; i < actual_popped; i++) {
					ss << std::to_string(m_tempBuffer[i]) << ", ";
				}
				std::cout << "wrote " << ss.str() << '\n';
			}
			m_totalBytes += bytes_transferred;
			do_write();
		}
	});
}
