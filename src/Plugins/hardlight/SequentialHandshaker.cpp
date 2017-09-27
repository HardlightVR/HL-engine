#include "stdafx.h"
#include "SequentialHandshaker.h"
#include "logger.h"
SequentialHandshaker::SequentialHandshaker(boost::asio::io_service & io, std::string portName)
	: m_io(io)
	, m_portName(portName)
	, m_handshakers()
	, m_onSuccess([](std::unique_ptr<boost::asio::serial_port> p ) { 
			boost::system::error_code ignored;
			p->close(ignored);
		})
	, m_onFail([]() {})
	, m_timeout(io)
{
}

void SequentialHandshaker::async_begin_handshake()
{
	if (m_handshakers.empty()) {
		m_onFail();
		return;
	}

	m_currentHandshaker = m_handshakers.begin();
	do_handshake();
}

void SequentialHandshaker::set_success_handler(SuccessHandler handler)
{
	m_onSuccess = std::move(handler);
}

void SequentialHandshaker::set_fail_handler(FailHandler handler)
{
	m_onFail = std::move(handler);
}

void SequentialHandshaker::add_handshaker(std::unique_ptr<Handshaker> handshaker)
{
	m_handshakers.push_back(std::move(handshaker));
}

void SequentialHandshaker::async_cancel()
{
	m_timeout.cancel();
	for (auto& shaker : m_handshakers) {
		shaker->cancel_timers_close_port();
	}
}

void SequentialHandshaker::handshaker_finished_handler()
{
	if ((*m_currentHandshaker)->status() == Handshaker::Status::Connected) {
		

		m_onSuccess((*m_currentHandshaker)->release());

	}
	else {
		m_currentHandshaker++;
		if (m_currentHandshaker == m_handshakers.end()) {
			m_onFail();
		}
		else {
			do_handshake();
		}
	}
}

void SequentialHandshaker::do_handshake()
{
	m_timeout.expires_from_now(boost::posix_time::millisec(500));
	m_timeout.async_wait([this](const auto& ec) {
		if (ec == boost::asio::error::operation_aborted) {
			return;
		}
		core_log("Handshaker", "Trying the next handler!");
		(*m_currentHandshaker)->set_finish_callback([this]() { handshaker_finished_handler(); });
		(*m_currentHandshaker)->start_handshake();
	});
	
}
