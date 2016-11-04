#pragma once
#include <boost\lockfree\spsc_queue.hpp>
class PacketDispatcher;

const unsigned int PACKET_LENGTH = 16;

struct packet
{
	uint8_t raw[PACKET_LENGTH];
	unsigned int size = PACKET_LENGTH;
};
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
	std::size_t PossiblePacketsAvailable();
	Synchronizer(std::shared_ptr<Buffer> dataStream, std::shared_ptr<PacketDispatcher> dispatcher);
	~Synchronizer();
private:
	State syncState;
	uint8_t packetDelimiter;
	uint8_t packetFooter[2];
	int badSyncCounter;
	const int BAD_SYNC_LIMIT = 2;
	std::shared_ptr<PacketDispatcher> _dispatcher;
	std::shared_ptr<Buffer> _dataStream;
	void searchForSync();
	void confirmSync();
	void monitorSync();
	void confirmSyncLoss();
	packet dequeuePacket() const;
	bool packetIsWellFormed(const packet p) const;
};

