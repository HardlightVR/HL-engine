#include "stdafx.h"
#include "MarkIIHandshaker.h"

const uint8_t MarkIIIHandshaker::pingData[7] = { 0x24, 0x02, 0x02, 0x07, 0xFF, 0xFF, 0x0A };

MarkIIIHandshaker::MarkIIIHandshaker(std::string name, boost::asio::io_service& io)
	: Handshaker(name, io)
{
}

void MarkIIIHandshaker::setup_port_options(boost::asio::serial_port & port)
{
	port.set_option(boost::asio::serial_port::baud_rate(9600));
	port.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
	port.set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::hardware));
	port.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
	port.set_option(boost::asio::serial_port::character_size(8));
}

const uint8_t * MarkIIIHandshaker::ping_data() const
{
	return pingData;
}

std::size_t MarkIIIHandshaker::ping_data_length() const
{
	return 7;
}

bool MarkIIIHandshaker::is_good_response(const uint8_t * data, unsigned int length) const
{
	if (length < 3) {
		return false;
	}

	return (
		data[0] == '$'
		&& data[1] == 0x02
		&& data[2] == 0x02
	);
}

const boost::posix_time::time_duration & MarkIIIHandshaker::read_timeout() const
{
	static auto timeout = boost::posix_time::millisec(500);
	return timeout;
}

const boost::posix_time::time_duration & MarkIIIHandshaker::write_timeout() const
{
	static auto timeout = boost::posix_time::millisec(500);
	return timeout;
}
