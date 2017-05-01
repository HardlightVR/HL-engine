#include "stdafx.h"
#include "AsyncTimeout.h"


AsyncTimeout::AsyncTimeout(boost::asio::io_service& io, boost::posix_time::time_duration timeout) :
	m_timeout(timeout), 
	m_expiryTimer(io)
{
}


AsyncTimeout::~AsyncTimeout()
{
}

void AsyncTimeout::OnTimeout(std::function<void(void)> fn)
{
	m_timeoutHandler = fn;
}



void AsyncTimeout::Go()
{
	m_expiryTimer.expires_from_now(m_timeout);
	m_expiryTimer.async_wait([this](auto error_code) {
		if (error_code) {
			//case 1: success of read, was canceled by the async_read
			//std::cout << "From cancel timer: " << error_code.message() << '\n';
		}
		else {
			
			m_timeoutHandler();
		}
	
	});

}

void AsyncTimeout::Cancel()
{
	try {
		m_expiryTimer.cancel();
	}
	catch (boost::system::system_error& ec) {
		std::cout << ec.what();
	}
}
