#include "Synchronizer.h"
#include <iostream>


bool Synchronizer::Synchronized()
{
	return this->syncState == Synchronizer::State::Synchronized;
}

Synchronizer::State Synchronizer::SyncState()
{
	return this->syncState;
}

void Synchronizer::TryReadPacket()
{
	//if (this->dataStream.Length < this.packetLength
	if (_dataStream->size() < PACKET_LENGTH) {
		return;
	}

	switch (this->syncState)
	{
	case Synchronizer::State::SearchingForSync:
		std::cout << "Searching for sync.." << "\n";
		this->searchForSync();
		break;
	case Synchronizer::State::ConfirmingSync:
		std::cout << "Confirming sync.." << "\n";

		this->confirmSync();
		break;
	case Synchronizer::State::Synchronized:

		this->monitorSync();
		break;
	case Synchronizer::State::ConfirmingSyncLoss:
		std::cout << "Confirming sync loss.." << "\n";

		this->confirmSyncLoss();
		break;
	default:
		break;
	}
}

unsigned const int Synchronizer::PACKET_LENGTH = 16;

struct packet
{
	uint8_t raw[Synchronizer::PACKET_LENGTH];
};
Synchronizer::Synchronizer(std::shared_ptr<CircularBuffer> dataStream, std::shared_ptr<PacketDispatcher> dispatcher) :
	_dispatcher(dispatcher),
	_dataStream(std::move(dataStream)),
	packetDelimiter('$'),
	syncState(Synchronizer::State::SearchingForSync),
	badSyncCounter(0),
	packetFooter{ 0x0D, 0x0A, }
{
}


Synchronizer::~Synchronizer()
{
}

void Synchronizer::searchForSync()
{
	//this->dataStream.Length < this->packetLength * 2
	if (this->_dataStream->size() < this->PACKET_LENGTH * 2) {
		return;
	}

	packet possiblePacket = dequeuePacket();
	if (this->packetIsWellFormed(possiblePacket)) {
		this->syncState = Synchronizer::State::ConfirmingSync;
		return;
	}

	//find offset 
	for (std::size_t offset = 1; offset < this->PACKET_LENGTH; ++offset) {
		if (possiblePacket.raw[offset] == this->packetDelimiter) {
			std::size_t howMuchLeft = this->PACKET_LENGTH - offset;
			for (std::size_t i = 0; i < PACKET_LENGTH; ++i)
			{
				_dataStream->pop_back();
			}
			this->syncState = State::ConfirmingSync;
			return;
		}
	}
}

void Synchronizer::confirmSync()
{
	packet possiblePacket = this->dequeuePacket();
	if (this->packetIsWellFormed(possiblePacket)) {
		this->syncState = State::Synchronized;
		std::cout << "synced" << "\n";
	}
	else {
		this->syncState = State::SearchingForSync;
	}
}

void Synchronizer::monitorSync()
{
	packet possiblePacket = this->dequeuePacket();
	if (this->packetIsWellFormed(possiblePacket)) {
		std::cout << "got a packet" << "\n";
	//	this->dispatcher.Dispatch(possiblePacket);
	}
	else {
		this->badSyncCounter = 1;
		this->syncState = Synchronizer::State::ConfirmingSyncLoss;
	}
}

void Synchronizer::confirmSyncLoss()
{
	packet possiblePacket = this->dequeuePacket();
	if (!this->packetIsWellFormed(possiblePacket)) {
		this->badSyncCounter++;
		if (this->badSyncCounter >= BAD_SYNC_LIMIT) {
			this->syncState = Synchronizer::State::SearchingForSync;
		}
	}
	else {
		this->syncState = Synchronizer::State::Synchronized;
	//	this->_dispatcher.Dispatch(possiblePacket);
	}
}

packet Synchronizer::dequeuePacket() const
{
	packet p;
		
	std::copy(_dataStream->end() - PACKET_LENGTH, _dataStream->end(), p.raw);
	for (std::size_t i = 0; i < PACKET_LENGTH; ++i)
	{
		_dataStream->pop_back();
	}
	return p;

}

bool Synchronizer::packetIsWellFormed(const packet possiblePacket) const
{
	return possiblePacket.raw[0] == packetDelimiter &&
		possiblePacket.raw[14] == packetFooter[0] &&
		possiblePacket.raw[15] == packetFooter[1];
}
