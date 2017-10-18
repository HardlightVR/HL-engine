#pragma once
#include <cstdint>
#include <boost\lockfree\spsc_queue.hpp>
#include <boost\asio\deadline_timer.hpp>
#include <boost\optional.hpp>
#include "PacketDispatcher.h"

#include "suit_packet.h"

using Buffer = boost::lockfree::spsc_queue<uint8_t>;


class Synchronizer
{
public:
	Synchronizer(Buffer& dataStream, PacketDispatcher& dispatcher, boost::asio::io_service&);
	enum class State {
		Synchronized,
		SearchingForSync,
		ConfirmingSync,
		ConfirmingSyncLoss
	};
	State SyncState() const;
	void BeginSync();
	std::size_t	GetTotalBytesRead() const;
private:
	std::size_t m_estimatedBytesRead;
	State m_syncState;
	uint8_t m_delimeter;
	uint8_t m_footer[2];
	std::size_t m_badSyncCounter;
	PacketDispatcher& m_dispatcher;
	Buffer& m_incomingData;
	boost::asio::deadline_timer m_syncTimer;
	boost::posix_time::milliseconds m_syncInterval;

	void scheduleSync();
	void searchForSync();
	void confirmSync();
	void monitorSync();
	void confirmSyncLoss();
	bool packetIsWellFormed(const boost::optional<packet>& possiblePacket) const;
	void tryReadPacket();

	boost::optional<packet> dequeuePacket();

};

bool IsSynchronized(const Synchronizer& s);

