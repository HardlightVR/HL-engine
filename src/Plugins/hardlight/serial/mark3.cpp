#include "stdafx.h"
#include "mark3.h"

mark3::mark3() : m_pingData({ 0x24, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x0D, 0x0A })
{
}

void mark3::set_options(boost::asio::serial_port * port) const
{
	port->set_option(boost::asio::serial_port::baud_rate(115200));
	port->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
	port->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::hardware));
	port->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
	port->set_option(boost::asio::serial_port::character_size(8));
}

const boost::asio::const_buffer mark3::do_get_ping_data() const
{
	return boost::asio::const_buffer(m_pingData.data(), m_pingData.size());
}
