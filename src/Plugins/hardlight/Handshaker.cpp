#include "stdafx.h"
#include "Handshaker.h"
#include "logger.h"

void Handshaker::start_handshake()
{
	m_io.post([this]() {async_open_port(); });
}

Handshaker::Handshaker(std::string name, boost::asio::io_service & io)
	: m_io(io)
	, m_name(name)
	, m_heartbeatTimer(io)
	, m_readTimer(io)
	, m_port(std::make_unique<boost::asio::serial_port>(io))
	, m_status(Status::Closed)
	, m_protocolFinished(false)
{
}

void Handshaker::set_finish_callback(FinishHandler onFinish)
{
	m_callback = onFinish;
}


Handshaker::Status Handshaker::status() const
{
	return m_status;
}

std::unique_ptr<boost::asio::serial_port> Handshaker::release()
{
	return std::move(m_port);
}

void Handshaker::cancel_timers_close_port()
{
	m_protocolFinished = true;
	m_heartbeatTimer.cancel();
	m_readTimer.cancel();
	boost::system::error_code ec;

	if (m_port && m_port->is_open()) {
		core_log("Handshaker", std::string("Shutting down " + m_name));

		m_port->close(ec);
		if (m_port->is_open()) {
			core_log(nsvr_severity_fatal, "Handshaker", "Really bad state: the port is open after it closed. Remember what you were doing to the suit when this happened. casey@hardlightvr.com");
		}
	}
}

bool Handshaker::is_finished() const
{
	return m_protocolFinished;
}

void Handshaker::async_open_port()
{
	core_log("SerialPort", std::string("Attempting to open " + m_name));
	assert(!m_port->is_open());
	boost::system::error_code ec;
	m_port->open(m_name, ec);

	if (ec) {
		core_log(nsvr_severity_error, "Handshaker", ec.message());
		cancel_timers_close_port();
		callback();
	}
	else {
		m_status = Status::Open;

		core_log("SerialPort", std::string("Setting port options on " + m_name));

		setup_port_options(*m_port);

		async_ping_port();
	}

}


void Handshaker::async_ping_port()
{
	core_log("Handshaker", std::string("Waiting on sending data to   " + m_name));
	m_heartbeatTimer.expires_from_now(write_timeout());
	m_heartbeatTimer.async_wait([this](const auto& ec) {check_write_deadline(ec); });
	m_port->async_write_some(boost::asio::buffer(ping_data(), ping_data_length()), [this](const auto& ec, auto bytes_transferred) { write_handler(ec, bytes_transferred); });

}



void Handshaker::write_handler(const boost::system::error_code & ec, std::size_t bytes_transferred)
{
	

	if (m_protocolFinished) {
		return;
	}
	m_heartbeatTimer.cancel();

	if (!m_port->is_open()) {
		m_status = Status::TimedOutWriting;
		m_protocolFinished = true;
		cancel_timers_close_port();
		callback();
	}
	else if (ec) {
		//there was an actual error writing to the port
		m_status = Status::Unwritable;
		m_protocolFinished = true;
		cancel_timers_close_port();
		callback();
	}
	else {
		start_read();
	}
}


void Handshaker::start_read()
{
	core_log("Handshaker", std::string("Waiting on response from  " + m_name));

	m_readTimer.expires_from_now(read_timeout());
	m_readTimer.async_wait([this](const auto& ec) {check_read_deadline(ec); });
	m_port->async_read_some(boost::asio::buffer(m_data), [this](const auto& ec, auto bytes_transferred) { read_handler(ec, bytes_transferred); });
}


void Handshaker::read_handler(const boost::system::error_code & ec, std::size_t bytes_transferred)
{

	if (m_protocolFinished) {
		return;
	}

	m_readTimer.cancel();

	if (!ec) {
		
		assert(m_port->is_open());
		m_status = is_good_response(m_data, bytes_transferred) ? Status::Connected : Status::BadReturnPing;
		core_log("Handshaker", std::string(m_name + ": " + (m_status == Status::Connected ? std::string("connected") : std::string("bad ping response."))));


		callback();

	}
	else {
		core_log("Handshaker", std::string(m_name + ": error reading: " + ec.message()));

		cancel_timers_close_port();
		callback();
	}
}



void Handshaker::check_write_deadline(const boost::system::error_code& ec)
{
	if (m_protocolFinished) {
		return;
	}
	if (m_heartbeatTimer.expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
		core_log("SerialPort", std::string("Write timeout expired on " + m_name + ", closing port"));
		m_status = Status::TimedOutWriting;
		boost::system::error_code ignored;
		m_port->close(ignored);
		m_heartbeatTimer.expires_at(boost::posix_time::pos_infin);
	}

}


void Handshaker::check_read_deadline(const boost::system::error_code& ec)
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

void Handshaker::callback()
{
	
	m_callback();
}
