#pragma once
#include <boost\asio\serial_port.hpp>
#include <boost\asio\io_service.hpp>
class Serial
{
public:
	Serial(boost::asio::io_service& io);
	~Serial();
	bool Connect(std::string portName);
	void Write(std::shared_ptr<uint8_t*> bytes, std::size_t length);
private:
	boost::asio::serial_port m_port;
};

