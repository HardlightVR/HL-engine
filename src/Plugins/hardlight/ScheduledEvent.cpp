#include "stdafx.h"

#include "ScheduledEvent.h"



ScheduledEvent::ScheduledEvent(boost::asio::io_service & io, boost::posix_time::time_duration interval):
	m_event([]() {}), 
	m_interval(interval),
	m_timer(io)
{
}

void ScheduledEvent::SetEvent(std::function<void(void)> someRegularEvent)
{
	m_event = someRegularEvent;
}

void ScheduledEvent::Start()
{
	m_timer.expires_from_now(m_interval);
	m_timer.async_wait([this](auto err) { handleEvent(err); });
	
}

void ScheduledEvent::Stop()
{
	m_timer.cancel();
}

ScheduledEvent::~ScheduledEvent()
{
}

void ScheduledEvent::handleEvent(const boost::system::error_code& ec)
{
	if (!ec) {
		m_event();
		this->Start();
	}
}
