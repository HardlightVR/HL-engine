#include "stdafx.h"
#include "mark3.h"

mark3::mark3() : m_pingData({ 0x24, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x0D, 0x0A })
{
}

void mark3::set_options(boost::asio::serial_port& port) const
{
	port.set_option(boost::asio::serial_port::baud_rate(115200));
	port.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
	port.set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::hardware));
	port.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
	port.set_option(boost::asio::serial_port::character_size(8));
}

bool mark3::is_valid_response(const std::array<uint8_t, 128>& buffer, std::size_t length) const
{
	if (length < 7) { return false; }

	return buffer[0] == '$' && buffer[1] == 0x02;

}

const boost::asio::const_buffer mark3::do_get_ping_data() const
{
	return boost::asio::const_buffer(m_pingData.data(), m_pingData.size());
}
