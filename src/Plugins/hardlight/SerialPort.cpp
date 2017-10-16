#include "stdafx.h"
#include "SerialPort.h"
#include "logger.h"
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


void SerialPort::stop()
{
	//core_log("SerialPort", std::string("Shutting down " + m_name));
	m_writeTimer.cancel();
	boost::system::error_code ec;

	if (m_port && m_port->is_open()) {
		m_port->close(ec);
		if (m_port->is_open()) {
			core_log(nsvr_severity_fatal, "SerialPort", "Really bad state: the port is open after it closed. Remember what you were doing to the suit when this happened. casey@hardlightvr.com");
		}
	}
}



//Make sure that the lifetime of the SerialPort object outlives the async operations. Either use
//enable shared from this, or take care of it in the adapter
SerialPort::SerialPort(std::string name, boost::asio::io_service & io, std::function<void()> doneFunc)
	: m_name(name)
	, m_io(io)
	, m_writeTimer(io)
	, m_readTimer(io)
	, m_writeTimeout(boost::posix_time::millisec(100))
	, m_readTimeout(boost::posix_time::millisec(250))
	, m_port(std::make_unique<boost::asio::serial_port>(io))
	, m_status(Status::Closed)
	, m_protocolFinished(false)
	, m_doneFunc(doneFunc)
{
	
}

void SerialPort::start_connect(std::shared_ptr<std::atomic<std::size_t>> num_tested_so_far, std::size_t total_amount)
{
	m_sentinel = num_tested_so_far;
	m_totalAmount = total_amount;

	m_io.post([this]() {async_open_port(); });
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
	core_log("SerialPort", std::string("Attempting to open " + m_name));
	
	boost::system::error_code ec;
	m_port->open(m_name, ec);

	if (ec) {
		core_log(nsvr_severity_error, "SerialPort", ec.message());
		m_protocolFinished = true;
		stop();
		check_if_all_ports_finished();
	}
	else {
		core_log("SerialPort", std::string("Setting port options on " + m_name));

		m_port->set_option(boost::asio::serial_port::baud_rate(9600));
		m_port->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
		m_port->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::hardware));
		m_port->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
		m_port->set_option(boost::asio::serial_port::character_size(8));

		m_status = Status::Open;
		async_ping_port(); 
	}

}

void SerialPort::async_ping_port()
{
	core_log("SerialPort", std::string("Handshake " + m_name));
	m_writeTimer.expires_from_now(m_writeTimeout);
	m_writeTimer.async_wait([this](const auto& ec) {check_write_deadline(ec); });
	m_port->async_write_some(boost::asio::buffer(pingData), [this](const auto& ec, auto bytes_transferred) { write_handler(ec, bytes_transferred); });
	
}


void SerialPort::write_handler(const boost::system::error_code & ec, std::size_t bytes_transferred)
{
	if (m_protocolFinished) {
		return;
	}

	if (!m_port->is_open()) {
		m_status = Status::TimedOutWriting;
		m_protocolFinished = true;
		stop();
		check_if_all_ports_finished();
	}
	else if (ec) {
		//there was an actual error writing to the port
		m_status = Status::Unwritable;
		m_protocolFinished = true;
		stop();
		check_if_all_ports_finished();
	}
	else {
		start_read();
	}
}


void SerialPort::check_if_all_ports_finished()
{
	//All of the SerialPort instances are sharing this sentinel value. When each finishes the connection routine,
	//we increment and then check against how many should have completed. If this value doesn't reach the total amount at some point,
	//we have a bug.

	//The last one is responsible for invoking the "done" function. 


	if (m_sentinel->fetch_add(1) + 1 == m_totalAmount) {
		m_doneFunc();
	}
	
	//It's very important that the last one be the one to call the doneFunc. Else, we would probably destroy the objects 
	//while their handlers are still active. That would be bad. We could probably design this with enable_shared_from_this().

}

void SerialPort::check_write_deadline(const boost::system::error_code& ec)
{
	if (m_protocolFinished) {
		return;
	}
	if (m_writeTimer.expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
		core_log("SerialPort", std::string("Write timeout expired on " + m_name + ", closing port"));
		m_status = Status::TimedOutWriting;
		boost::system::error_code ignored;
		m_port->close(ignored);
		m_writeTimer.expires_at(boost::posix_time::pos_infin);
	}

}


void SerialPort::check_read_deadline(const boost::system::error_code& ec)
{
	if (m_protocolFinished) {
		return;
	}

	if (m_readTimer.expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
		core_log("SerialPort", std::string("Read timeout expired on " + m_name + ", closing port"));
		m_status = Status::TimedOutReading;
		boost::system::error_code ignored;
		m_port->close(ignored);
		m_readTimer.expires_at(boost::posix_time::pos_infin);
	}

}

void SerialPort::start_read()
{
	core_log("SerialPort", std::string("Waiting on response from  " + m_name));

	m_readTimer.expires_from_now(m_readTimeout);
	m_readTimer.async_wait([this](const auto& ec) {check_read_deadline(ec); });
	m_port->async_read_some(boost::asio::buffer(m_data), [this](const auto& ec, auto bytes_transferred) { read_handler(ec, bytes_transferred); });
}


void SerialPort::read_handler(const boost::system::error_code & ec, std::size_t bytes_transferred)
{

	if (m_protocolFinished) {
		return;
	}

	if (!ec) {
		m_protocolFinished = true;
		m_writeTimer.cancel();
		m_status = IsPingPacket(m_data, bytes_transferred) ? Status::Connected : Status::BadReturnPing;
		core_log("SerialPort", std::string(m_name + ": " + (m_status == Status::Connected ? std::string("connected") : std::string("bad ping response."))));
		
		check_if_all_ports_finished();
	}
	else {
		m_protocolFinished = true;
		stop();
		check_if_all_ports_finished();
	}
	

	


}
