#pragma once


#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/asio/deadline_timer.hpp>
#include "HardwareIO.h"

enum class RetryStatus {
	Unknown = 0,
	InProgress = 1,
	UserCanceled = 2,
	ExceededMaxTries = 3
};
class RetrySender {
public:
	RetrySender(boost::asio::io_service& io, std::vector<uint8_t> packetToSend, HardwareIO* out);

	void begin();
	void end();

	RetryStatus get_status() const;
private:
	boost::posix_time::time_duration m_delay;
	boost::asio::deadline_timer m_timer;
	std::vector<uint8_t>  m_packet;
	HardwareIO* m_outgoing;
	size_t m_currentTry;
	RetryStatus m_status;
	void do_write();
};