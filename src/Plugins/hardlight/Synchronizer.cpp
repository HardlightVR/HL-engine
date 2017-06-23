#include "StdAfx.h"
#include "Synchronizer.h"
#include <iostream>
#include <boost\log\trivial.hpp>
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
	if (_dataStream->read_available() < PACKET_LENGTH){
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

void Synchronizer::BeginSync()
{
	scheduleSync();
}
void Synchronizer::scheduleSync()
{
	_syncTimer.expires_from_now(_syncInterval);
	_syncTimer.async_wait([&](const boost::system::error_code& ec) {
		handleReadPacket(ec);
	});
}
void Synchronizer::handleReadPacket(const boost::system::error_code& ec) {
	this->TryReadPacket();
	scheduleSync();
}


std::size_t Synchronizer::PossiblePacketsAvailable()
{
	return _dataStream->read_available() / PACKET_LENGTH;
}



Synchronizer::Synchronizer(std::shared_ptr<Buffer> dataStream, PacketDispatcher& dispatcher, boost::asio::io_service& io) :
	_dispatcher(dispatcher),
	_dataStream(dataStream),
	packetDelimiter('$'),
	syncState(Synchronizer::State::SearchingForSync),
	badSyncCounter(0),
	packetFooter{ 0x0D, 0x0A, },
	_syncInterval(10),
	_syncTimer(io)
{
}


Synchronizer::~Synchronizer()
{
}

void Synchronizer::searchForSync()
{
	//this->dataStream.Length < this->packetLength * 2
	if (this->_dataStream->read_available() < PACKET_LENGTH * 2) {
		return;
	}

	packet possiblePacket = dequeuePacket();
	if (this->packetIsWellFormed(possiblePacket)) {
		this->syncState = Synchronizer::State::ConfirmingSync;
		return;
	}

	//find offset 
	for (std::size_t offset = 1; offset < PACKET_LENGTH; ++offset) {
		if (possiblePacket.raw[offset] == this->packetDelimiter) {
			std::size_t howMuchLeft = offset;
			for (std::size_t i = 0; i < howMuchLeft; ++i)
			{
				_dataStream->pop();
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
		BOOST_LOG_TRIVIAL(info) << "[Sync] Stream synced";

	}
	else {
		this->syncState = State::SearchingForSync;
	}
}

void Synchronizer::monitorSync()
{
	packet possiblePacket = this->dequeuePacket();
	if (this->packetIsWellFormed(possiblePacket)) {
		_dispatcher.Dispatch(possiblePacket);

	}
	else {
		this->badSyncCounter = 1;
		this->syncState = Synchronizer::State::ConfirmingSyncLoss;

		BOOST_LOG_TRIVIAL(info) << "[Sync] Confirming sync loss..";

	}
}

void Synchronizer::confirmSyncLoss()
{
	
	packet possiblePacket = this->dequeuePacket();
	if (!this->packetIsWellFormed(possiblePacket)) {
		this->badSyncCounter++;
		if (this->badSyncCounter >= BAD_SYNC_LIMIT) {
			BOOST_LOG_TRIVIAL(info) << "[Sync] Sync loss confirmed, searching..";

			this->syncState = Synchronizer::State::SearchingForSync;
		}
	}
	else {
		BOOST_LOG_TRIVIAL(info) << "[Sync] Achieved sync";

		this->syncState = Synchronizer::State::Synchronized;
	//	this->_dispatcher.Dispatch(possiblePacket);
	}
}

packet Synchronizer::dequeuePacket() const
{
	packet p;


	try
	{
		int numPopped = _dataStream->pop(p.raw, PACKET_LENGTH);
		assert(numPopped == PACKET_LENGTH);
	
	}
	catch (const std::exception& ) {
		BOOST_LOG_TRIVIAL(info) << "[Sync] Tried to read from stream but wasn't enough data";

	}
	return p;

}

bool Synchronizer::packetIsWellFormed(const packet& possiblePacket) const
{
	return possiblePacket.raw[0] == packetDelimiter &&
		possiblePacket.raw[14] == packetFooter[0] &&
		possiblePacket.raw[15] == packetFooter[1];
}



