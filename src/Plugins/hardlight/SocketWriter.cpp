#include "stdafx.h"
#include "SocketWriter.h"

SocketWriter::SocketWriter(boost::lockfree::spsc_queue<uint8_t>& outgoing, boost::asio::serial_port & port)
{
}
