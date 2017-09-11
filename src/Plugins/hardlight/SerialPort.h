#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>
#include "AsyncTimeout.h"



bool IsPingPacket(uint8_t* data, std::size_t length);

class SerialPort {


public:
	enum class Status {
		Unknown,
		Closed,
		Open,
		Connected,
		Disconnected,
		Unreadable,
		Unwritable
	};
	SerialPort(std::string name, boost::asio::io_service& io, std::function<void()>);
	void async_init_connection_process(std::shared_ptr<std::size_t> num_tested_so_far, std::size_t total_amount);
	Status status() const;
	std::unique_ptr<boost::asio::serial_port> release();
private:
	const static uint8_t pingData[7];

	Status m_status;
	std::string m_name;
	std::unique_ptr<boost::asio::serial_port> m_port;
	boost::asio::io_service& m_io;

	boost::asio::deadline_timer m_pingTimer;
	boost::posix_time::milliseconds m_pingTimeout;

	//Size of our incoming data buffer - should be tailored to the serial device
	const static unsigned int INCOMING_DATA_BUFFER_SIZE = 128;

	//our incoming data buffer 
	uint8_t m_data[INCOMING_DATA_BUFFER_SIZE];
	std::shared_ptr<std::size_t> m_sentinel;
	std::size_t m_totalAmount;

	std::function<void()> m_doneFunc;
	void async_open_port();
	void async_ping_port();
	void async_wait_response();
	void read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred);
	void write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred);
	void finish_protocol();
};