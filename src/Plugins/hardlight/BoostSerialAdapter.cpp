#include "StdAfx.h"
#include "BoostSerialAdapter.h"
#include <iostream>
#include "enumser.h"
#include "Locator.h"
#include "AsyncTimeout.h"
#include <boost/optional.hpp>
#include "logger.h"
#include <atomic>
#include "Mark2Handshaker.h"
#include "Mark3Handshaker.h"

uint8_t BoostSerialAdapter::m_pingData[7] = { 0x24, 0x02, 0x02, 0x07, 0xFF, 0xFF, 0x0A };

BoostSerialAdapter::BoostSerialAdapter(boost::asio::io_service& io) :
	m_outputSuitData(std::make_shared<Buffer>(4096)), 
	m_port(nullptr),
	m_io(io),
	m_suitReconnectionTimeout(boost::posix_time::milliseconds(1000)),
	m_initialConnectTimeout(boost::posix_time::milliseconds(3000)),
	m_suitReconnectionTimer(io),
	m_handshakers()
{
	std::fill(m_data, m_data + INCOMING_DATA_BUFFER_SIZE, 0);
}

void BoostSerialAdapter::Connect()
{
	beginReconnectionProcess();
}



void BoostSerialAdapter::beginReconnectionProcess() {
	if (m_port && m_port->is_open()) {
		m_port->cancel();
		boost::system::error_code ignored;
		m_port->close(ignored);
	}
	core_log(nsvr_severity_info, "SerialAdapter", "Disconnected from suit");
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
		core_log(nsvr_severity_info, "SerialAdapter", "No ports available on the system");
	}

	assert(m_handshakers.empty());

	std::shared_ptr<std::size_t> numPorts = std::make_shared<std::size_t>(0);
	auto finished_handshakers = std::make_shared<std::size_t>(0);

	for (std::size_t i = 0; i < ports.size(); ++i) {
		std::string portName = "COM" + std::to_string(ports[i]);

		auto handshaker = std::make_unique<SequentialHandshaker>(m_io, portName);
		//should probably be a factory, so add_handshaker("mark-3")

		handshaker->add_handshaker(std::make_unique<Mark3Handshaker>(portName, m_io));
		handshaker->add_handshaker(std::make_unique<Mark2Handshaker>(portName, m_io));

		handshaker->set_fail_handler([sentinel = finished_handshakers, total = ports.size(), this]() {
			(*sentinel)++;
			if (*sentinel == total) {
				core_log("SerialAdapter", "The entire run of sequential handshakers is done");
				m_io.post([this]() {
					m_handshakers.clear();
					scheduleDelayedSuitReconnect();

				});

			}
			else {
				core_log("SerialAdapter", "A handshaker has finished but failed to find anything");

			}
		});
		handshaker->set_success_handler([this](std::unique_ptr<boost::asio::serial_port> p) {
			core_log("SerialAdapter", "A handshaker has finished successfully");
			m_port = std::move(p);
			assert(m_port->is_open());

			for (auto& hs : m_handshakers) {
				hs->async_cancel();
			}
			assert(m_port->is_open());

			m_io.post([this]() {
				assert(m_port->is_open());
				m_handshakers.clear();
				assert(m_port->is_open());

				endReconnectionProcess();

			});
		});

		m_handshakers.push_back(std::move(handshaker));
	
		
	}

	//The way this works is that the slowest connecting port is the gatekeeper to actually connecting. The upper bound here is
	//just the sum of the read and write timeouts, so not long (~1 second), but we could do better by selecting the first available port.
	//A better design would probably instantiate a new suit immediately upon connecting, and then instantiate more as they are recognized.
	//This is easier for now, because I can manage the lifetime of the ports by keeping them all around until the last succeeds or fails.

	//auto numPortsToTest = std::make_shared<std::atomic<std::size_t>>(0);
	//std::size_t total = m_handshakers.size();
	for (auto& shaker : m_handshakers) {
		shaker->async_begin_handshake();
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

	/*std::unique_ptr<boost::asio::serial_port> possiblePort;
	for (auto& candidate : m_handshakers) {
		if (candidate->status() == Handshaker::Status::Connected) {
			possiblePort = candidate->release();
		}
		else {
			candidate->stop();
		}
	}

	for (auto& candidate : m_handshakers) {
		assert(candidate->is_finished());
	}

	m_handshakers.clear();

	if (possiblePort) {
		m_port = std::move(possiblePort);
		endReconnectionProcess();
	}
	else {
		scheduleDelayedSuitReconnect();
	}
*/
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
	if (!m_port) {
		int a = 2;
	}
	if (!m_port->is_open()) {
		int b = 3;
	}
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








