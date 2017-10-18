#pragma once

#include "Handshaker.h"
class Mark3Handshaker : public Handshaker {
public:
	Mark3Handshaker(std::string name, boost::asio::io_service& io);
private:
	void setup_port_options(boost::asio::serial_port& port) override;
	const uint8_t* ping_data() const override;
	std::size_t ping_data_length() const override;
	bool is_good_response(const uint8_t* data, unsigned int length) const override;
	const static uint8_t pingData[7];
	const boost::posix_time::time_duration& read_timeout() const override;
	const boost::posix_time::time_duration& write_timeout() const override;
};