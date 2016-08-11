#include "Synchronizer.h"


struct packet
{
	uint8_t raw[8];
};

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
	if (_dataStream) {
		return;
	}

	switch (this->syncState)
	{
	case Synchronizer::State::SearchingForSync:
		this->searchForSync();
		break;
	case Synchronizer::State::ConfirmingSync:
		this->confirmSync();
		break;
	case Synchronizer::State::Synchronized:
		this->monitorSync();
		break;
	case Synchronizer::State::ConfirmingSyncLoss:
		this->confirmSyncLoss();
		break;
	default:
		break;
	}
}


Synchronizer::Synchronizer(std::shared_ptr<CircularBuffer> dataStream, std::shared_ptr<PacketDispatcher> dispatcher) :
	_dispatcher(dispatcher),
	_dataStream(dataStream),
	packetLength(16),
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
	if (this->_dataStream->size() < this->packetLength * 2) {
		return;
	}

	uint8_t* possiblePacket = dequeuePacket();
	if (this->packetIsWellFormed(possiblePacket)) {
		this->syncState = Synchronizer::State::ConfirmingSync;
		return;
	}

	//find offset 
	for (std::size_t offset = 1; offset < this->packetLength; ++offset) {
		if (possiblePacket[offset] == this->packetDelimiter) {
			std::size_t howMuchLeft = this->packetLength - offset;
			//this->dataStream.Dequeue(possiblePacket, 0, howMuchLeft);
			this->syncState = State::ConfirmingSync;
			return;
		}
	}
}

void Synchronizer::confirmSync()
{
	uint8_t* possiblePacket = this->dequeuePacket();
	if (this->packetIsWellFormed(possiblePacket)) {
		this->syncState = State::Synchronized;
	}
	else {
		this->syncState = State::SearchingForSync;
	}
}

void Synchronizer::monitorSync()
{
	uint8_t* possiblePacket = this->dequeuePacket();
	if (this->packetIsWellFormed(possiblePacket)) {
	//	this->dispatcher.Dispatch(possiblePacket);
	}
	else {
		this->badSyncCounter = 1;
		this->syncState = Synchronizer::State::ConfirmingSyncLoss;
	}
}

void Synchronizer::confirmSyncLoss()
{
	uint8_t* possiblePacket = this->dequeuePacket();
	if (!this->packetIsWellFormed(possiblePacket)) {
		this->badSyncCounter++;
		if (this->badSyncCounter >= BAD_SYNC_LIMIT) {
			this->syncState = Synchronizer::State::SearchingForSync;
		}
	}
	else {
		this->syncState = Synchronizer::State::Synchronized;
		//this->dispatcher.Dispatch(possiblePacket);
	}
}

packet Synchronizer::dequeuePacket()
{
	packet p;
		
	std::copy(_dataStream->end(), _dataStream->end() - 8, p.raw);
	for (std::size_t i = 0; i < packetLength; ++i)
	{
		_dataStream->pop_back();
	}
	return p;

}

bool Synchronizer::packetIsWellFormed(uint8_t * packet)
{
	return false;
}
