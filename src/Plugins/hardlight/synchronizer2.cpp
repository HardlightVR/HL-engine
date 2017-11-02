#include "stdafx.h"
#include "synchronizer2.h"

#include "PacketDispatcher.h"
const uint8_t PACKET_DELIMETER = '$';
const uint8_t PACKET_FOOTER[2] = { 0x0D, 0x0A };

bool packetIsWellFormed(const Packet& actualPacket)
{
	


	return  actualPacket[0] == PACKET_DELIMETER &&
		actualPacket[14] == PACKET_FOOTER[0] &&
		actualPacket[15] == PACKET_FOOTER[1];
}


synchronizer2::State synchronizer2::state() const
{
	return m_state;
}

synchronizer2::synchronizer2(boost::asio::io_service& io, boost::lockfree::spsc_queue<uint8_t>& data)
	: m_dispatcher()
	, m_data(data)
	, m_badSyncLimit(2)
	, m_badSyncCounter(0)
	, m_syncInterval(boost::posix_time::millisec(10))
	, m_syncTimer(io)
	, m_totalBytesRead(0)
	, m_state(State::SearchingForSync)
{

}

void synchronizer2::start()
{
	search_for_sync();
}

void synchronizer2::stop()
{
	m_syncTimer.cancel();
}

void synchronizer2::on_packet(PacketEvent::slot_type slot)
{
	m_dispatcher.connect(slot);
}

std::size_t synchronizer2::total_bytes_read() const
{
	return m_totalBytesRead;
}

void synchronizer2::confirm_sync()
{
	m_state = State::ConfirmingSync;


	auto self(shared_from_this());


	auto possiblePacket = dequeuePacket();
	if (!possiblePacket) {
		m_syncTimer.expires_from_now(m_syncInterval);
		m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; } confirm_sync(); });
		return;
	}

	if (packetIsWellFormed(*possiblePacket)) {
		m_dispatcher(*possiblePacket);
		m_syncTimer.expires_from_now(m_syncInterval);
		m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; } synchronized_read(); });
	}
	else {
		m_syncTimer.expires_from_now(m_syncInterval);
		m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; }  search_for_sync(); });
	}
}

void synchronizer2::confirm_sync_loss()
{
	m_state = State::ConfirmingSyncLoss;


	auto self(shared_from_this());

	auto possiblePacket = this->dequeuePacket();
	if (!possiblePacket) {
		m_syncTimer.expires_from_now(m_syncInterval);
		m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; }  confirm_sync_loss(); });
		return;
	}

	if (!packetIsWellFormed(*possiblePacket)) {
		m_badSyncCounter++;
		if (m_badSyncCounter >= m_badSyncLimit) {
			//core_log("Synchronizer", "Sync loss confirmed, searching for sync..");
			m_syncTimer.expires_from_now(m_syncInterval);
			m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; }  search_for_sync(); });
		}
		else {
			m_syncTimer.expires_from_now(m_syncInterval);
			m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; }  confirm_sync_loss(); });
		}
	}
	else {
		m_dispatcher(*possiblePacket);
		//core_log("Synchronizer", "Sync achieved.");
		m_syncTimer.expires_from_now(m_syncInterval);
		m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; }  synchronized_read(); });
	}

	
}

void synchronizer2::search_for_sync()
{
	m_state = State::SearchingForSync;

	auto self(shared_from_this());

	/*if (m_data.read_available() < PACKET_LENGTH * 2) {
		m_syncTimer.expires_from_now(m_syncInterval);
		m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; }  search_for_sync(); });
		return;
	}*/
	
	auto possiblePacket = dequeuePacket();
	if (!possiblePacket) {
		m_syncTimer.expires_from_now(m_syncInterval);
		m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; }  search_for_sync(); });
		return;
	}

	if (packetIsWellFormed(*possiblePacket) || seek_offset(*possiblePacket)) {
		m_syncTimer.expires_from_now(m_syncInterval);
		m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; } confirm_sync(); });
		return;
	}

	m_syncTimer.expires_from_now(m_syncInterval);
	m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; }  search_for_sync(); });
}

bool synchronizer2::seek_offset(const Packet& realPacket)
{

	for (std::size_t offset = 1; offset < PACKET_LENGTH; ++offset) {
		if (realPacket[offset] == PACKET_DELIMETER) {
			std::size_t howMuchLeft = offset;
			for (std::size_t i = 0; i < howMuchLeft; ++i)
			{
				m_data.pop();
			}
			return true;
		}
	}

	return false;
}

void synchronizer2::synchronized_read()
{
	m_state = State::Synchronized;

	auto self(shared_from_this());

	auto possiblePacket = this->dequeuePacket();

	if (!possiblePacket) {
		m_syncTimer.expires_from_now(m_syncInterval);
		m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; } synchronized_read(); });
		return;
	}

	if (packetIsWellFormed(*possiblePacket)) {
		m_dispatcher(*possiblePacket);
		m_syncTimer.expires_from_now(m_syncInterval);
		m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; } synchronized_read(); });
	}
	else {
		m_badSyncCounter = 1;
		m_syncTimer.expires_from_now(m_syncInterval);
		m_syncTimer.async_wait([self, this](auto ec) { if (ec) { return; } confirm_sync_loss(); });
	}
}



boost::optional<Packet> synchronizer2::dequeuePacket()
{
	try
	{
		auto avail = m_data.read_available();
		if (avail < PACKET_LENGTH) {
			return boost::none;
		}


		Packet p;
		int numPopped = m_data.pop(p.data(), PACKET_LENGTH);
		assert(numPopped == PACKET_LENGTH);
		m_totalBytesRead += numPopped;


		return p;

	}
	catch (const std::exception&) {
		//core_log(nsvr_severity_error, "Synchronizer", std::string("Tried to read from the data stream, but there wasn't enough data!" + std::string(e.what())));
	}

	return boost::none;

}