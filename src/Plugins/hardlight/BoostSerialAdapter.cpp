#include "StdAfx.h"
#include <iostream>
#include <atomic>
#include <boost/optional.hpp>

#include "BoostSerialAdapter.h"
#include "enumser.h"
#include "Locator.h"
#include "AsyncTimeout.h"
#include "logger.h"
#include "Mark2Handshaker.h"
#include "Mark3Handshaker.h"
#include "PacketVersion.h"

BoostSerialAdapter::BoostSerialAdapter(boost::asio::io_service& io) 
	: m_io(io)
	, m_heartbeat()
	, m_port()
	, m_incomingSuitData(4096) 
	, m_isReconnecting(false)
	, m_onPacketVersionChange()
	, m_suitReconnectionTimeout(boost::posix_time::milliseconds(1000))
	, m_suitReconnectionTimer(io)
	, m_handshakers()
{
	std::fill(std::begin(m_suitReadBuffer), std::end(m_suitReadBuffer), 0);
}

void BoostSerialAdapter::ConnectAsync()
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
	m_isReconnecting = true;
	scheduleImmediateSuitReconnect();
}

void BoostSerialAdapter::scheduleImmediateSuitReconnect()
{
	m_suitReconnectionTimer.expires_from_now(boost::posix_time::millisec(10));
	m_suitReconnectionTimer.async_wait([this](auto err) { testAllPorts(err); });
}

struct handshake_complete_sentinel {
	const std::size_t total_needed;
	std::size_t current_finished;
	handshake_complete_sentinel(std::size_t total_needed) : total_needed(total_needed), current_finished(0) {}
};

template<typename SuccessHandler, typename FailHandler>
std::unique_ptr<SequentialHandshaker> make_handshaking_algorithm(boost::asio::io_service& io, SuccessHandler&& success, FailHandler&& fail) {
	auto handshaker = std::make_unique<SequentialHandshaker>(io);
	handshaker->set_success_handler(std::move(success));
	handshaker->set_fail_handler(std::move(fail));
	return std::move(handshaker);
}


std::vector<std::string> fetchAllSerialPortNames() {
	CEnumerateSerial::CPortsArray ports;
	if (!CEnumerateSerial::UsingQueryDosDevice(ports)) {
		core_log(nsvr_severity_info, "SerialAdapter", "No ports available on the system");
	}
	std::vector<std::string> portNames = {};
	for (unsigned int port : ports) {
		portNames.push_back("COM" + std::to_string(port));
	}
	return portNames;
}

void BoostSerialAdapter::testAllPorts(const boost::system::error_code& ec) {
	if (ec) {
		return;
	}

	std::vector<std::string> ports = fetchAllSerialPortNames();

	assert(m_handshakers.empty());
	
	auto finished_handshaking = std::make_shared<handshake_complete_sentinel>(ports.size());

	for (const auto& portName : ports) {
		
		auto hs = make_handshaking_algorithm(m_io, 
			//Success handler
			[this](std::unique_ptr<boost::asio::serial_port> p, PacketVersion version) {
				core_log("SerialAdapter", "A handshaker has finished successfully");
				m_port = std::move(p);
				for (auto& hs : m_handshakers) { hs->async_cancel(); }
				m_io.post([this, version]() {
					m_handshakers.clear();
					assert(m_port->is_open());

					//first, tell the firmware interface to use the correct packet version (pinging will depend on this, etc.)
					m_onPacketVersionChange(version);
					endReconnectionProcess();
				});

			//Fail handler
			}, [sentinel = finished_handshaking, this]() {
				sentinel->current_finished++;
				if (sentinel->current_finished == sentinel->total_needed) {
					core_log("SerialAdapter", "The entire run of sequential handshakers is done");
					m_io.post([this]() {
						m_handshakers.clear();
						scheduleDelayedSuitReconnect();
					});
				}
			});

		hs->add_handshaker(std::make_unique<Mark3Handshaker>(portName, m_io));
		hs->add_handshaker(std::make_unique<Mark2Handshaker>(portName, m_io));
		m_handshakers.push_back(std::move(hs));	
	}

	for (auto& shaker : m_handshakers) {
		shaker->async_begin_handshake();
	}
}




void BoostSerialAdapter::Disconnect()
{
	boost::system::error_code ignored;
	if (m_port->is_open()) {
		m_port->close(ignored);
	}
}

void BoostSerialAdapter::Write(std::shared_ptr<uint8_t*> bytes, std::size_t length, WriteHandler&& write_handler)
{
	if (this->m_port && this->m_port->is_open()) {
		this->m_port->async_write_some(boost::asio::buffer(*bytes, length), std::move(write_handler));
	}
}




void BoostSerialAdapter::scheduleDelayedSuitReconnect()
{
	m_suitReconnectionTimer.expires_from_now(m_suitReconnectionTimeout);
	m_suitReconnectionTimer.async_wait([this](auto err) { testAllPorts(err); });
}





void BoostSerialAdapter::endReconnectionProcess()
{
	m_isReconnecting = false;
	assert(m_heartbeat);
	m_heartbeat->BeginListening();
	
	kickoffSuitReading();
}



void BoostSerialAdapter::kickoffSuitReading()
{

	if (!IsConnected()) { return;  }

	m_port->async_read_some(boost::asio::buffer(m_suitReadBuffer, INCOMING_DATA_BUFFER_SIZE), [this]
		(const auto& error, auto bytes_transferred) {
			if (!error) {
				
				m_incomingSuitData.push(m_suitReadBuffer, bytes_transferred);
				std::fill(m_suitReadBuffer, m_suitReadBuffer + INCOMING_DATA_BUFFER_SIZE, 0);
		
				kickoffSuitReading();
			}	
		}
	);
}





Buffer& BoostSerialAdapter::GetDataStream()
{
	return m_incomingSuitData;
}

bool BoostSerialAdapter::IsConnected() const
{
	return  !m_isReconnecting &&  this->m_port && this->m_port->is_open();
}

void BoostSerialAdapter::SetConnectionMonitor(std::shared_ptr<Heartbeat> monitor)
{
	m_heartbeat = monitor;	
	m_heartbeat->OnDisconnect([this]() { beginReconnectionProcess(); });

}

void BoostSerialAdapter::OnPacketVersionChange(std::function<void(PacketVersion)> fn)
{
	m_onPacketVersionChange = fn;
}


BoostSerialAdapter::~BoostSerialAdapter()
{
	Disconnect();
}








