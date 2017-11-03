#include "stdafx.h"
#include "ReaderAdapter.h"

ReaderAdapter::ReaderAdapter(boost::lockfree::spsc_queue<uint8_t>& incoming,boost::asio::serial_port& port)
	: m_incoming(incoming)
	, m_port(port)
{
}

void ReaderAdapter::start()
{
	do_read();
}


void ReaderAdapter::do_read()
{
	auto self(shared_from_this());

	m_port.async_read_some(boost::asio::buffer(m_tempBuffer, INCOMING_DATA_BUFFER_SIZE), [self, this](auto ec, std::size_t bytes_transferred) {
		if (!ec) {
			m_incoming.push(m_tempBuffer.data(), bytes_transferred);
			do_read();
		}
	});
}
