#pragma once
#include "StdAfx.h"
#include "PacketDispatcher.h"
#include <boost/circular_buffer.hpp>

struct packet;

typedef boost::circular_buffer<uint8_t> CircularBuffer;
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
	const static unsigned int PACKET_LENGTH;

	Synchronizer(std::shared_ptr<CircularBuffer> dataStream, std::shared_ptr<PacketDispatcher> dispatcher);
	~Synchronizer();
private:
	State syncState;
	uint8_t packetDelimiter;
	uint8_t packetFooter[2];
	int badSyncCounter;
	const int BAD_SYNC_LIMIT = 2;
	std::shared_ptr<PacketDispatcher> _dispatcher;
	std::shared_ptr<CircularBuffer> _dataStream;
	void searchForSync();
	void confirmSync();
	void monitorSync();
	void confirmSyncLoss();
	packet dequeuePacket() const;
	bool packetIsWellFormed(const packet p) const;
};

