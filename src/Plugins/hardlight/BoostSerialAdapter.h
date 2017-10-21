#pragma once
#include <boost\lockfree\spsc_queue.hpp>
#include "KeepaliveMonitor.h"
#include <boost/asio/serial_port.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Synchronizer.h"
#include "SerialPort.h"
#include "SequentialHandshaker.h"
#include "PacketVersion.h"


//Note: serial ports are a total pain. Using boost::asio, they are slightly less of a pain.
//This class, and the classes that it is composed of, may want to be refactored. Unfortunately it is hard to test without real hardware,
//which means it is hard to change without significant retesting. 


class BoostSerialAdapter
{
public:
	using WriteHandler = std::function<void(const boost::system::error_code&, std::size_t)>;

	explicit BoostSerialAdapter(boost::asio::io_service& io);
	~BoostSerialAdapter();

	void Connect();
	void Disconnect();
	void Write(std::shared_ptr<uint8_t*> bytes, std::size_t length, WriteHandler&& write_handler = [](const auto& ec, std::size_t) {});

	Buffer& GetDataStream();

	bool IsConnected() const;

	void SetConnectionMonitor(std::shared_ptr<KeepaliveMonitor> monitor);

	//only one handler can be set
	//this is bad architecture and hacky for now
	void OnPacketVersionChange(std::function<void(PacketVersion)>);

private:
	std::function<void(PacketVersion)> m_onPacketVersionChange;
	boost::asio::io_service& m_io;

	std::unique_ptr<boost::asio::serial_port> m_port;

	const static unsigned int INCOMING_DATA_BUFFER_SIZE = 128;

	uint8_t m_tempSuitData[INCOMING_DATA_BUFFER_SIZE];
	
	Buffer m_filteredSuitData;

	bool m_isReconnecting = false;

	//Handles disconnect logic, e.g. if the suit doesn't send a ping in 1 sec, tell us to reconnect
	std::shared_ptr<KeepaliveMonitor> m_keepaliveMonitor;

	void kickoffSuitReading();

	void testAllPorts(const boost::system::error_code& ec);

	void scheduleDelayedSuitReconnect();

	void scheduleImmediateSuitReconnect();
	
	void beginReconnectionProcess();

	void endReconnectionProcess();

	boost::asio::deadline_timer m_suitReconnectionTimer;

	boost::posix_time::milliseconds m_suitReconnectionTimeout;

	std::vector<std::unique_ptr<SequentialHandshaker>> m_handshakers;

};

