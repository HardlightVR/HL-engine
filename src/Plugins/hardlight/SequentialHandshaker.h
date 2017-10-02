#pragma once

#include <boost/asio/io_service.hpp>
#include <string>
#include "Handshaker.h"
class SequentialHandshaker {
public:
	using SuccessHandler = std::function<void(std::unique_ptr<boost::asio::serial_port>)>;
	using FailHandler = std::function<void()>;
	SequentialHandshaker(boost::asio::io_service& io);
	void async_begin_handshake();
	void set_success_handler(SuccessHandler handler);
	void set_fail_handler(FailHandler handler);

	void add_handshaker(std::unique_ptr<Handshaker> handshaker);

	void async_cancel();
private:
	boost::asio::io_service& m_io;
	std::vector<std::unique_ptr<Handshaker>> m_handshakers;
	decltype(m_handshakers)::iterator m_currentHandshaker;
	SuccessHandler m_onSuccess;
	FailHandler m_onFail;
	boost::asio::deadline_timer m_timeout;
	void handshaker_finished_handler();
	void do_handshake();
};