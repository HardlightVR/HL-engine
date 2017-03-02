#pragma once
class ScheduledEvent
{
public:
	ScheduledEvent(boost::asio::io_service& io, boost::posix_time::time_duration interval);
	void SetEvent(std::function<void(void)> someRegularEvent);
	void Start();
	void Stop();
	~ScheduledEvent();
private:
	boost::posix_time::time_duration m_interval;
	std::function<void(void)> m_event;

	boost::asio::deadline_timer m_timer;

	void handleEvent(const boost::system::error_code& ec);
};

