#pragma once

#include "Handshaker.h"
class MarkIIHandshaker : public Handshaker {
public:
	MarkIIHandshaker();
private:
	void setup_port_options(boost::asio::serial_port& port);
	const uint8_t* ping_data() const;
	std::size_t ping_data_length() const;
	bool is_good_response(const uint8_t* data, unsigned int length) const;
	const static uint8_t pingData[7];
	const boost::posix_time::time_duration& read_timeout() const;
	const boost::posix_time::time_duration& write_timeout() const;
};