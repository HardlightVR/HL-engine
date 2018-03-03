#pragma once
#include <memory>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost\lockfree\spsc_queue.hpp>
#include <boost/optional.hpp>

#include "suit_packet.h"
#include <boost/signals2.hpp>

class PacketDispatcher;
class synchronizer2   {
public:
	enum class State {
		Synchronized,
		SearchingForSync,
		ConfirmingSync,
		ConfirmingSyncLoss
	};
	State state() const;

	using PacketEvent = boost::signals2::signal<void(Packet)>;
	synchronizer2(boost::asio::io_service& io, boost::lockfree::spsc_queue<uint8_t>& data);
	void start();
	void stop();

	void on_packet(PacketEvent::slot_type slot);

	std::size_t	total_bytes_read() const;

	boost::asio::io_service& get_io_service();

private:
	void transition_state();
	void confirm_sync();
	void confirm_sync_loss();
	void search_for_sync();
	bool seek_offset(const Packet& possiblePacket);
	void synchronized_read();

	boost::optional<Packet> synchronizer2::dequeuePacket();

	PacketEvent m_dispatcher;

	boost::lockfree::spsc_queue<uint8_t>& m_data;

	const std::size_t m_badSyncLimit;

	std::size_t m_badSyncCounter;

	boost::posix_time::millisec m_syncInterval;

	boost::asio::deadline_timer m_syncTimer;

	std::size_t m_totalBytesRead;

	State m_state;



};

