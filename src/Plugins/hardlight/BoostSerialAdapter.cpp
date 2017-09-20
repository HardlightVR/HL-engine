#include "StdAfx.h"
#include "BoostSerialAdapter.h"
#include <iostream>
#include <future>
#include <boost/asio/use_future.hpp>
#include "enumser.h"
#include "Locator.h"
#include "AsyncTimeout.h"
#include <boost/optional.hpp>
#include "logger.h"
#include <atomic>

uint8_t BoostSerialAdapter::m_pingData[7] = { 0x24, 0x02, 0x02, 0x07, 0xFF, 0xFF, 0x0A };

BoostSerialAdapter::BoostSerialAdapter(boost::asio::io_service& io) :
	m_outputSuitData(std::make_shared<Buffer>(4096)), 
	m_port(nullptr),
	m_io(io),
	m_suitReconnectionTimeout(boost::posix_time::milliseconds(50)),
	m_initialConnectTimeout(boost::posix_time::milliseconds(3000)),
	m_suitReconnectionTimer(io)
{
	std::fill(m_data, m_data + INCOMING_DATA_BUFFER_SIZE, 0);
}

void BoostSerialAdapter::Connect()
{
	beginReconnectionProcess();
}



void BoostSerialAdapter::beginReconnectionProcess() {
	core_log(nsvr_loglevel_info, "SerialAdapter", "Disconnected from suit");
	_isResetting = true;
	scheduleImmediateSuitReconnect();
}

void BoostSerialAdapter::scheduleImmediateSuitReconnect()
{
	m_suitReconnectionTimer.expires_from_now(boost::posix_time::millisec(10));
	m_suitReconnectionTimer.async_wait([this](auto err) { testAllPorts(err); });
}



void BoostSerialAdapter::testAllPorts(const boost::system::error_code& ec) {
	if (ec) {
		return;
	}

	//First, we retrieve all the available ports. This is windows-only. 
	CEnumerateSerial::CPortsArray ports;
	CEnumerateSerial::CNamesArray names;
	if (!CEnumerateSerial::UsingQueryDosDevice(ports)) {
		core_log(nsvr_loglevel_info, "SerialAdapter", "No ports available on the system");
	}

	m_candidatePorts.clear();

	for (std::size_t i = 0; i < ports.size(); ++i) {
		std::string portName = "COM" + std::to_string(ports[i]);
		m_candidatePorts.push_back(std::make_unique<SerialPort>(portName, m_io, [this]() { findBestPort(); }));
	}

	//The way this works is that the slowest connecting port is the gatekeeper to actually connecting. The upper bound here is
	//just the sum of the read and write timeouts, so not long (~1 second), but we could do better by selecting the first available port.
	//A better design would probably instantiate a new suit immediately upon connecting, and then instantiate more as they are recognized.
	//This is easier for now, because I can manage the lifetime of the ports by keeping them all around until the last succeeds or fails.

	auto numPortsToTest = std::make_shared<std::atomic<std::size_t>>(0);
	std::size_t total = m_candidatePorts.size();
	for (auto& port : m_candidatePorts) {
		port->async_init_connection_process(numPortsToTest, total);
	}
}




void BoostSerialAdapter::Disconnect()
{
	try {
		if (m_port->is_open()) {
			m_port->close();
		}
	}
	catch (const boost::system::error_code&) {
		//log this
	}
}

void BoostSerialAdapter::Write(std::shared_ptr<uint8_t*> bytes, std::size_t length, std::function<void(const boost::system::error_code&, std::size_t)> cb)
{
	if (this->m_port && this->m_port->is_open()) {
		this->m_port->async_write_some(boost::asio::buffer(*bytes, length), cb);
	}
}

void BoostSerialAdapter::Write(std::shared_ptr<uint8_t*> bytes, std::size_t length)
{
	if (this->m_port && this->m_port->is_open()) {
		this->m_port->async_write_some(boost::asio::buffer(*bytes, length),
			[bytes](const boost::system::error_code& error, std::size_t bytes_transferred) {
		});
	}
}

void BoostSerialAdapter::findBestPort()
{

	std::unique_ptr<boost::asio::serial_port> possiblePort;
	for (auto& candidate : m_candidatePorts) {
		if (candidate->status() == SerialPort::Status::Connected) {
			possiblePort = candidate->release();
			break;
		}
	}

	m_candidatePorts.clear();

	if (possiblePort) {
		m_port = std::move(possiblePort);
		endReconnectionProcess();
	}
	else {
		scheduleDelayedSuitReconnect();
	}

}

void BoostSerialAdapter::scheduleDelayedSuitReconnect()
{
	m_suitReconnectionTimer.expires_from_now(m_suitReconnectionTimeout);
	m_suitReconnectionTimer.async_wait([this](auto err) { testAllPorts(err); });
}





void BoostSerialAdapter::endReconnectionProcess()
{
	_isResetting = false;
	assert(m_keepaliveMonitor);
	m_keepaliveMonitor->BeginMonitoring();

	kickoffSuitReading();
}



void BoostSerialAdapter::kickoffSuitReading()
{
	if (!m_port || !m_port->is_open()) {
		return;
	}

	m_port->async_read_some(boost::asio::buffer(m_data, INCOMING_DATA_BUFFER_SIZE), [this]
		(const auto& error, auto bytes_transferred) {
			if (!error) {
				if (!IsPingPacket(m_data, bytes_transferred)) {
					//if it's not a ping packet, put it into the data stream. Don't want pings cluttering stuff up.
					m_outputSuitData->push(m_data, bytes_transferred);
					std::fill(m_data, m_data + INCOMING_DATA_BUFFER_SIZE, 0);
				}
				else {
					assert(m_keepaliveMonitor);
					m_keepaliveMonitor->ReceivePing();
				}
				kickoffSuitReading();
			}	
		}
	);
}





std::shared_ptr<Buffer> BoostSerialAdapter::GetDataStream()
{
	return this->m_outputSuitData;
}

bool BoostSerialAdapter::IsConnected() const
{
	return  !_isResetting &&  this->m_port && this->m_port->is_open();
}

void BoostSerialAdapter::SetMonitor(std::shared_ptr<KeepaliveMonitor> monitor)
{
	m_keepaliveMonitor = monitor;	
	m_keepaliveMonitor->OnDisconnect([this]() { beginReconnectionProcess(); });

}


BoostSerialAdapter::~BoostSerialAdapter()
{
	Disconnect();
}








