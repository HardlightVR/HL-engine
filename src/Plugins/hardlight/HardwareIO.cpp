#include "stdafx.h"
#include "HardwareIO.h"


constexpr std::size_t INCOMING_CAPACITY = 4096;
constexpr std::size_t OUTGOING_CAPACITY = 4096 * 3 ;

HardwareIO::HardwareIO(std::unique_ptr<boost::asio::serial_port> port)
	: m_port(std::move(port))
	, m_incoming(std::make_shared<boost::lockfree::spsc_queue<uint8_t>>(INCOMING_CAPACITY))
	, m_outgoing(std::make_shared<boost::lockfree::spsc_queue<uint8_t>>(OUTGOING_CAPACITY))
	, m_reader(std::make_shared<ReaderAdapter>(m_incoming, *m_port))
	, m_writer(std::make_shared<WriterAdapter>(m_outgoing, *m_port))
{
	
}

std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> HardwareIO::incoming_queue()
{
	return m_incoming;
}

std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> HardwareIO::outgoing_queue()
{
	return m_outgoing;
}

void HardwareIO::start()
{
	m_reader->start();
	m_writer->start();
}

void HardwareIO::stop()
{
	m_reader->stop();
	m_writer->stop();
	boost::system::error_code ignored;
	m_port->close(ignored);

}

std::size_t HardwareIO::outgoing_queue_capacity() const
{
	return OUTGOING_CAPACITY;
}

std::size_t HardwareIO::outgoing_queue_size() const
{
	return m_outgoing->read_available();
}

std::size_t HardwareIO::bytes_read() const
{
	return m_reader->total_bytes_read();
}

std::size_t HardwareIO::bytes_written() const
{
	return m_writer->total_bytes_written();
}
