#include "stdafx.h"
#include "SerialPort.h"
#include <boost/log/trivial.hpp>

const uint8_t SerialPort::pingData[7] = { 0x24, 0x02, 0x02, 0x07, 0xFF, 0xFF, 0x0A };



bool IsPingPacket(uint8_t* data, std::size_t length)
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

//Make sure that the lifetime of the SerialPort object outlives the async operations. Either use
//enable shared from this, or take care of it in the adapter
SerialPort::SerialPort(std::string name, boost::asio::io_service & io, std::function<void()> doneFunc)
	: m_name(name)
	, m_io(io)
	, m_pingTimer(io)
	, m_pingTimeout(boost::posix_time::millisec(500))
	, m_port(std::make_unique<boost::asio::serial_port>(io))
	, m_status(Status::Closed)
	, m_doneFunc(doneFunc)
{
}

void SerialPort::async_init_connection_process(std::shared_ptr<std::size_t> num_tested_so_far, std::size_t total_amount)
{
	m_sentinel = num_tested_so_far;
	m_totalAmount = total_amount;

	m_io.post([this]() { async_open_port();  });
}

SerialPort::Status SerialPort::status() const
{
	return m_status;
}

std::unique_ptr<boost::asio::serial_port> SerialPort::release()
{
	return std::move(m_port);
}

void SerialPort::async_open_port()
{
	boost::system::error_code ec;
	m_port->open(m_name, ec);

	m_port->set_option(boost::asio::serial_port::baud_rate(115200));
	m_port->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
	m_port->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
	m_port->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
	m_port->set_option(boost::asio::serial_port::character_size(8));


	if (ec) {
		m_status = Status::Closed;
		finish_protocol();
	}
	else {
		m_status = Status::Open;
		m_io.post([this]() { async_ping_port(); });
	}
}

void SerialPort::async_ping_port()
{
	m_port->async_write_some(boost::asio::buffer(pingData), [this](const auto& ec, auto bytes_transferred) { write_handler(ec, bytes_transferred); });
}


void SerialPort::write_handler(const boost::system::error_code & ec, std::size_t bytes_transferred)
{
	if (ec) {
		m_status = Status::Unwritable;
		finish_protocol();
	}
	else {
		m_io.post([this]() { async_wait_response(); });
	}
}


void SerialPort::finish_protocol()
{
	//All of the SerialPort instances are sharing this sentinel value. When each finishes the connection routine,
	//we increment and then check against how many should have completed. If this value doesn't reach the total amount at some point,
	//we have a bug.

	//The last one is responsible for invoking the "done" function. 

	(*m_sentinel)++;


	if (*m_sentinel == m_totalAmount) {
		m_doneFunc();
	}
	
	//It's very important that the last one be the one to call the donefunc. Else, we would probably destroy the objects 
	//while their handlers are still active. That would be bad. We could probably design this with enable_shared_from_this().

}

void SerialPort::async_wait_response()
{
	m_port->async_read_some(boost::asio::buffer(m_data), [this](const auto& ec, auto bytes_transferred) { read_handler(ec, bytes_transferred); });
	m_pingTimer.expires_from_now(m_pingTimeout);
	m_pingTimer.async_wait([this](const boost::system::error_code& ec) {
		if (ec == boost::asio::error::operation_aborted) {
			//canceled, because of success :D
			BOOST_LOG_TRIVIAL(trace) << "[SerialPort] Timer was canceled, good news!";
		}
		else if (ec) {
			//some other error?
			BOOST_LOG_TRIVIAL(trace) << "[SerialPort] Timer was canceled, but it wasn't us that did it..";
			m_status = Status::Unknown;
			finish_protocol();
		}	
		else {
			//the timer expired. Aka, no good. 
			BOOST_LOG_TRIVIAL(trace) << "[SerialPort] Timer expired. No good.";
			m_port->close();
			m_status = Status::Unreadable;
			finish_protocol();
		}
	});
}

void SerialPort::read_handler(const boost::system::error_code & ec, std::size_t bytes_transferred)
{
	//This happens if we do something like close the port. The timeout watchdog might say "hey, time's up." 
	//In that case, we just need to return.
	if (ec == boost::asio::error::operation_aborted) {
		return; 
	}


	//At this point, we want to cancel the timer because we have received a response. It might not be what we want, or it might 
	//be an error saying "don't have permission to read this port", but it's not a timeout. 
	m_pingTimer.cancel();

	//Some generic error that we don't know about?
	if (ec) {
		m_status = Status::Unreadable;
		finish_protocol();
		return;
	}

	m_status = IsPingPacket(m_data, bytes_transferred) ? Status::Connected : Status::Disconnected;
	
	finish_protocol();
}
