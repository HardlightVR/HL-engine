#pragma once
#include <boost\lockfree\spsc_queue.hpp>
#include <boost\asio\deadline_timer.hpp>
#include "PacketDispatcher.h"

#include "suit_packet.h"

typedef boost::lockfree::spsc_queue<uint8_t> Buffer;
class Synchronizer
{
public:
	enum class State {
		Synchronized,
		SearchingForSync,
		ConfirmingSync,
		ConfirmingSyncLoss
	};
	bool Synchronized();
	State SyncState();
	void TryReadPacket();
	void BeginSync();
	std::size_t PossiblePacketsAvailable();
	Synchronizer(Buffer& dataStream, PacketDispatcher& dispatcher, boost::asio::io_service&);
	~Synchronizer();
private:
	State syncState;
	const int MAX_PACKET_READ = 500;
	uint8_t packetDelimiter;
	uint8_t packetFooter[2];
	int badSyncCounter;
	const int BAD_SYNC_LIMIT = 2;
	PacketDispatcher& _dispatcher;
	Buffer& _dataStream;
	void searchForSync();
	void confirmSync();
	void monitorSync();
	void confirmSyncLoss();
	packet dequeuePacket() const;
	bool packetIsWellFormed(const packet& p) const;
	boost::asio::deadline_timer _syncTimer;
	boost::posix_time::milliseconds _syncInterval;

	void scheduleSync();
	void handleReadPacket(const boost::system::error_code& ec);

};

/*
Synchronizers job:
1. Check if packets are available
2. If the amount of packets is < N, read them and dispatch. Go to step 1.
3. Else, read N packets, and leave for a few ms
4. Go to step 2

Serials job:
1. Attempt to read from the port.
2. If the read times out, increment the bad read counter and check value. If < 3, go to 4. Else, go to 3.
3. If the bad read counter >= 3, try to reconnect
4. Copy the data to the internal buffer, schedule another read (1)

*/