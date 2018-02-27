#include "stdafx.h"
#include "HardwareIO.h"


constexpr std::size_t INCOMING_CAPACITY = 4096;
constexpr std::size_t OUTGOING_CAPACITY = 4096 * 3 ;

HardwareIO::HardwareIO()
	
	: m_incoming(std::make_shared<boost::lockfree::spsc_queue<uint8_t>>(INCOMING_CAPACITY))
	, m_outgoing(std::make_shared<boost::lockfree::spsc_queue<uint8_t>>(OUTGOING_CAPACITY))

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
	do_interface_creation();

}

void HardwareIO::stop()
{
	do_interface_cleanup();
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
	//return total_bytes_read();
	return get_reader()->total_bytes_read();
}

std::size_t HardwareIO::bytes_written() const
{
	return get_writer()->total_bytes_written();
}
