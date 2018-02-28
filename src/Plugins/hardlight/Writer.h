#pragma once
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/asio.hpp>
//Increase to 128 when Mark2 is discontinued and not supported
constexpr std::size_t serial_buffer_size = 64;


template<typename IoObject>
class Writer {
public:
	Writer(boost::lockfree::spsc_queue<uint8_t>& outgoing, IoObject& port);
	void start() ;
	void stop() ;

	std::size_t total_bytes_written() const;
private:
	bool m_stopped;
	boost::lockfree::spsc_queue<uint8_t>& m_outgoing;
	std::array<uint8_t, serial_buffer_size> m_tempBuffer;

	IoObject& m_port;
	boost::posix_time::millisec m_delay;
	boost::asio::deadline_timer m_timer;
	void do_write();

	std::size_t m_totalBytes;

};


template<typename IoObject>
inline Writer<IoObject>::Writer(boost::lockfree::spsc_queue<uint8_t>& outgoing, IoObject & port)
	: m_outgoing(outgoing)
	, m_port(port)
	, m_stopped(false)
	, m_totalBytes(0)
	, m_delay(boost::posix_time::millisec(5))
	, m_timer(port.get_io_service())
{
}



template<typename IoObject>
inline void Writer<IoObject>::start()
{
	do_write();
}

template<typename IoObject>
inline void Writer<IoObject>::stop()
{
	m_stopped = true;
}



template<typename IoObject>
inline std::size_t Writer<IoObject>::total_bytes_written() const
{
	return m_totalBytes;
}

template<typename IoObject>

inline void Writer<IoObject>::do_write()
{

	auto available_bytes = m_outgoing.read_available();

	std::size_t amount_to_write = std::min<std::size_t>(m_tempBuffer.size(), available_bytes);

	assert(available_bytes % 16 == 0);


	const auto actual_popped = m_outgoing.pop(m_tempBuffer.data(), amount_to_write);
	assert(actual_popped <= m_tempBuffer.size());
	boost::asio::async_write(m_port, boost::asio::buffer(m_tempBuffer.data(), actual_popped), [this, actual_popped](auto ec, std::size_t bytes_transferred) {

		if (!ec) {
			/*if (actual_popped > 0) {
			std::stringstream ss;
			for (int i = 0; i < actual_popped; i++) {
			ss << std::to_string(m_tempBuffer[i]) << ", ";
			}
			std::cout << "wrote " << ss.str() << '\n';
			}*/
			m_totalBytes += bytes_transferred;

			m_timer.expires_from_now(m_delay);
			m_timer.async_wait([this](auto ec) { if (ec) { return; } if (m_stopped) { return; } do_write(); });
		}
	});
}
