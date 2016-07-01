#pragma once
#include "StdAfx.h"
#include "ByteQueue.h"
#include "PacketDispatcher.h"
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

	Synchronizer(std::shared_ptr<ByteQueue> dataStream, std::shared_ptr<PacketDispatcher> dispatcher);
	~Synchronizer();
private:
	State syncState;
	int packetLength;
	uint8_t packetDelimiter;
	uint8_t packetFooter[2];
	int badSyncCounter;
	const int BAD_SYNC_LIMIT = 2;
	std::shared_ptr<PacketDispatcher> dispatcher;
	std::shared_ptr<ByteQueue> dataStream;
	void searchForSync();
	void confirmSync();
	void monitorSync();
	void confirmSyncLoss();
	uint8_t* dequeuePacket();
	bool packetIsWellFormed(uint8_t* packet);
};

