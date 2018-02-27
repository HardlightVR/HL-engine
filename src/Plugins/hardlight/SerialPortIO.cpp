#include "stdafx.h"
#include "SerialPortIO.h"
#include "SerialPortReader.h"
#include "SerialPortWriter.h"
SerialPortIO::SerialPortIO(std::unique_ptr<boost::asio::serial_port> port) : HardwareIO(), m_port(std::move(port))
{
	m_reader = std::make_shared<SerialPortReader>(incoming_queue(), *m_port);
	m_writer = std::make_shared<SerialPortWriter>(outgoing_queue(), *m_port);

}


void SerialPortIO::do_interface_creation() {
	m_reader->start();
	m_writer->start();

}
void SerialPortIO::do_interface_cleanup()
{
	m_reader->stop();
	m_writer->stop();

	boost::system::error_code ignored;
	m_port->close(ignored);
}

ReaderAdapter * SerialPortIO::get_reader() const
{
	return m_reader.get();
}

WriterAdapter * SerialPortIO::get_writer() const
{
	return m_writer.get();
}
