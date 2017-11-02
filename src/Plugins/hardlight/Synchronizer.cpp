#include "StdAfx.h"
#include "Synchronizer.h"
#include <iostream>
#include "logger.h"
#include "PacketDispatcher.h"

const int BAD_SYNC_LIMIT = 2;


Synchronizer::Synchronizer(Buffer& dataStream, PacketDispatcher& dispatcher, boost::asio::io_service& io) :
	m_dispatcher(dispatcher),
	m_incomingData(dataStream),
	m_delimeter('$'),
	m_syncState(Synchronizer::State::SearchingForSync),
	m_badSyncCounter(0),
	m_footer{ 0x0D, 0x0A, },
	m_syncInterval(10),
	m_syncTimer(io),
	m_estimatedBytesRead(0)
{
}

void Synchronizer::BeginSync()
{
	scheduleSync();
}

void Synchronizer::StopSync() {
	m_syncTimer.cancel();
}


void Synchronizer::scheduleSync()
{
	m_syncTimer.expires_from_now(m_syncInterval);
	m_syncTimer.async_wait([&](const boost::system::error_code& ec) {
		//Seemingly innocent addition of this line
		//The intent is to return if the io service tells this handler to stop because we are shutting down
		if (ec) { return; }
		tryReadPacket();
		scheduleSync();
	});
}


void Synchronizer::tryReadPacket()
{
	if (m_incomingData.read_available() < PACKET_LENGTH){
		return;
	}
	

	switch (this->m_syncState)
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

//AHHH the bug is that the sync can't sync unless PACKET_LENGTH * 2 has been rec'd. todo: fix.
void Synchronizer::searchForSync()
{
	if (m_incomingData.read_available() < PACKET_LENGTH * 2) {
		return;
	}

	auto possiblePacket = dequeuePacket();
	if (this->packetIsWellFormed(possiblePacket)) {
		this->m_syncState = Synchronizer::State::ConfirmingSync;
		return;
	}

	//find offset 
	const auto& realPacket = *possiblePacket;
	
	for (std::size_t offset = 1; offset < PACKET_LENGTH; ++offset) {
		if (realPacket[offset] == m_delimeter) {
			std::size_t howMuchLeft = offset;
			for (std::size_t i = 0; i < howMuchLeft; ++i)
			{
				m_incomingData.pop();
			}
			this->m_syncState = State::ConfirmingSync;
			return;
		}
	}
}


boost::optional<Packet> Synchronizer::dequeuePacket()
{
	try
	{
		Packet p;
		int numPopped = m_incomingData.pop(p.data(), PACKET_LENGTH);
		assert(numPopped == PACKET_LENGTH);
		m_estimatedBytesRead += numPopped;

		for (int i = 0; i < p.size(); i++) {
			std::cout << std::to_string(p[i] )<< " ";
		}
		std::cout << '\n';
		return p;

	}
	catch (const std::exception& e) {
		core_log(nsvr_severity_error, "Synchronizer", std::string("Tried to read from the data stream, but there wasn't enough data!" + std::string(e.what())));
	}
	
	return boost::none;

}

void Synchronizer::confirmSync()
{
	auto possiblePacket = this->dequeuePacket();
	if (this->packetIsWellFormed(possiblePacket)) {
		this->m_syncState = State::Synchronized;
		m_dispatcher.Dispatch(*possiblePacket);
		core_log("Synchronizer", "Stream synced");
	}
	else {
		this->m_syncState = State::SearchingForSync;
	}
}

void Synchronizer::monitorSync()
{
	auto possiblePacket = this->dequeuePacket();
	if (this->packetIsWellFormed(possiblePacket)) {
		m_dispatcher.Dispatch(*possiblePacket);

	}
	else {
		this->m_badSyncCounter = 1;
		this->m_syncState = Synchronizer::State::ConfirmingSyncLoss;
		core_log("Synchronizer", "Confirming sync loss..");
	}
}

void Synchronizer::confirmSyncLoss()
{

	auto possiblePacket = this->dequeuePacket();
	if (!this->packetIsWellFormed(possiblePacket)) {
		this->m_badSyncCounter++;
		if (this->m_badSyncCounter >= BAD_SYNC_LIMIT) {
			core_log("Synchronizer", "Sync loss confirmed, searching for sync..");

			this->m_syncState = Synchronizer::State::SearchingForSync;
		}
	}
	else {
		core_log("Synchronizer", "Sync achieved.");
		this->m_syncState = Synchronizer::State::Synchronized;
	}
}


Synchronizer::State Synchronizer::SyncState() const
{
	return this->m_syncState;
}


std::size_t Synchronizer::GetTotalBytesRead() const
{
	return m_estimatedBytesRead;
}


bool Synchronizer::packetIsWellFormed(const boost::optional<Packet>& possiblePacket) const
{
	if (!possiblePacket) {
		return false;
	}

	auto& actualPacket = *possiblePacket;

	return  actualPacket[0] == m_delimeter &&
			actualPacket[14] == m_footer[0] &&
			actualPacket[15] == m_footer[1];
	}

bool IsSynchronized(const Synchronizer & s)
{
	return s.SyncState() == Synchronizer::State::Synchronized;
}
