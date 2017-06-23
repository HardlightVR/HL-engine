#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <functional>

class AsyncTimeout
{
public:
	
	AsyncTimeout(boost::asio::io_service& io, boost::posix_time::time_duration timeout);
	~AsyncTimeout();

	void OnTimeout(std::function<void(void)> fn);
	void Go();
	void Cancel();
private:
	boost::posix_time::time_duration m_timeout;
	std::function<void(void)> m_timeoutHandler;
	boost::asio::deadline_timer m_expiryTimer;
};

