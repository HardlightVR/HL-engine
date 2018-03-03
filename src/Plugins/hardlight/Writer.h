#pragma once
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/asio.hpp>
//Increase to 128 when Mark2 is discontinued and not supported


template<typename IoObject, size_t packet_size>
class Writer {
public:
	
	Writer::Writer(boost::lockfree::spsc_queue<uint8_t>& outgoing, IoObject & port)
		: m_outgoing(outgoing)
		, m_port(port)
		, m_stopped(false)
		, m_totalBytes(0)
		, m_delay(boost::posix_time::millisec(5))
		, m_timer(port.get_io_service()) {}


	void Writer::start() { 
		do_write(); 
	}

	void Writer::stop() {
		m_stopped = true;
	}


	std::size_t Writer::total_bytes_written() const {
		return m_totalBytes;
	}


	void Writer::do_write() {

		const auto available_bytes = m_outgoing.read_available();
		assert(available_bytes % 16 == 0);

		const auto amount_to_write = std::min(packet_size, available_bytes);
		const auto actual_popped = m_outgoing.pop(m_tempBuffer.data(), amount_to_write);

		assert(actual_popped <= packet_size);


		boost::asio::async_write(m_port, boost::asio::buffer(m_tempBuffer.data(), actual_popped), [this, actual_popped](auto ec, std::size_t bytes_transferred) {
			if (!ec) {
				m_totalBytes += bytes_transferred;
				schedule_write();
			}
			else {
				std::cout << "Failed to write!\n";
			}
		});
	}

	void schedule_write() {
		m_timer.expires_from_now(m_delay);
		m_timer.async_wait([this](auto ec) { if (ec) { return; } if (m_stopped) { return; } do_write(); });
	}

private:
	bool m_stopped;
	boost::lockfree::spsc_queue<uint8_t>& m_outgoing;
	std::array<uint8_t, packet_size> m_tempBuffer;

	IoObject& m_port;
	boost::posix_time::millisec m_delay;
	boost::asio::deadline_timer m_timer;

	std::size_t m_totalBytes;

};

