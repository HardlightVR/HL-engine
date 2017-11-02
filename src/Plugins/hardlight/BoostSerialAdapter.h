#pragma once
#include <boost\lockfree\spsc_queue.hpp>
#include "Heartbeat.h"
#include <boost/asio/serial_port.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Synchronizer.h"
#include "SerialPort.h"
#include "SequentialHandshaker.h"
#include "PacketVersion.h"
#include "serial/connection_info.h"


class BoostSerialAdapter
{
public:
	using WriteHandler = std::function<void(const boost::system::error_code&, std::size_t)>;

	explicit BoostSerialAdapter(boost::asio::io_service& io);
	~BoostSerialAdapter();

	void ConnectAsync();
	void Disconnect();

	void Connect(connection_info info);

	void Write(uint8_t* bytes, std::size_t length, WriteHandler&&);

	Buffer& GetDataStream();

	bool IsConnected() const;

	void SetConnectionMonitor(std::shared_ptr<Heartbeat> monitor);

	//only one handler can be set
	//this is bad architecture and hacky for now
	void OnPacketVersionChange(std::function<void(PacketVersion)>);

private:
	boost::asio::io_service& m_io;

	std::shared_ptr<Heartbeat> m_heartbeat;

	std::unique_ptr<boost::asio::serial_port> m_port;

	constexpr static int INCOMING_DATA_BUFFER_SIZE = 128;

	uint8_t m_suitReadBuffer[INCOMING_DATA_BUFFER_SIZE];
	
	Buffer m_incomingSuitData;

	bool m_isReconnecting = false;

	std::function<void(PacketVersion)> m_onPacketVersionChange;

	boost::asio::deadline_timer m_suitReconnectionTimer;

	boost::posix_time::milliseconds m_suitReconnectionTimeout;

	std::vector<std::unique_ptr<SequentialHandshaker>> m_handshakers;

	void kickoffSuitReading();

	void testAllPorts(const boost::system::error_code& ec);

	void scheduleDelayedSuitReconnect();

	void scheduleImmediateSuitReconnect();
	
	void beginReconnectionProcess();

	void endReconnectionProcess();

	

};

