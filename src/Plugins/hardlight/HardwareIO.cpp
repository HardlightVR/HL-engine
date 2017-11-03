#include "stdafx.h"
#include "HardwareIO.h"



HardwareIO::HardwareIO(std::unique_ptr<boost::asio::serial_port> port)
	: m_port(std::move(port))
	, m_incoming(std::make_shared<boost::lockfree::spsc_queue<uint8_t>>(4096))
	, m_outgoing(std::make_shared<boost::lockfree::spsc_queue<uint8_t>>(4096))
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
	std::cout << "They called stop on HWIo\n";
	boost::system::error_code ignored;
	m_port->close(ignored);

}
