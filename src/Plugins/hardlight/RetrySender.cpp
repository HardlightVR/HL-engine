#include "stdafx.h"
#include "RetrySender.h"


constexpr size_t MAX_TRIES = 3;

RetrySender::RetrySender(boost::asio::io_service& io, std::vector<uint8_t> packetToSend, boost::lockfree::spsc_queue<uint8_t>* outgoing_queue)
	: m_delay(boost::posix_time::milliseconds(400))
	, m_timer(io)
	, m_packet(packetToSend)
	, m_outgoing(outgoing_queue)
	, m_currentTry(0)
	, m_status(RetryStatus::Unknown)

{}


void RetrySender::begin()
{
	m_status = RetryStatus::InProgress;
	do_write();
}



void RetrySender::end()
{
	m_timer.cancel();
	m_status = RetryStatus::UserCanceled;

}

RetryStatus RetrySender::get_status() const
{
	return m_status;
}


void RetrySender::do_write() {
	m_currentTry++;

	m_outgoing->push(m_packet.data(), m_packet.size());

	if (m_currentTry < MAX_TRIES) {
		m_timer.expires_from_now(m_delay);
		m_timer.async_wait([this](auto ec) { if (ec) { return; } do_write(); });
	}
	else {
		m_status = RetryStatus::ExceededMaxTries;
	}
}