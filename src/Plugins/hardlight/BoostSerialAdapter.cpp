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


BoostSerialAdapter::BoostSerialAdapter(boost::asio::io_service& io) :
	m_filteredSuitData(4096), 
	m_port(nullptr),
	m_io(io),
	m_suitReconnectionTimeout(boost::posix_time::milliseconds(1000)),
	m_suitReconnectionTimer(io),
	m_handshakers()
{
	std::fill(m_tempSuitData, m_tempSuitData + INCOMING_DATA_BUFFER_SIZE, 0);
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
	for (std::size_t i = 0; i < ports.size(); i++) {
		portNames.push_back("COM" + std::to_string(ports[i]));
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
			[this](std::unique_ptr<boost::asio::serial_port> p) {
				core_log("SerialAdapter", "A handshaker has finished successfully");
				m_port = std::move(p);
				for (auto& hs : m_handshakers) { hs->async_cancel(); }
				m_io.post([this]() {
					m_handshakers.clear();
					assert(m_port->is_open());
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
	try {
		if (m_port->is_open()) {
			m_port->close();
		}
	}
	catch (const boost::system::error_code&) {
		//log this
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
	assert(m_keepaliveMonitor);
	m_keepaliveMonitor->BeginMonitoring();

	kickoffSuitReading();
}



void BoostSerialAdapter::kickoffSuitReading()
{

	if (!m_port || !m_port->is_open()) {
		return;
	}

	m_port->async_read_some(boost::asio::buffer(m_tempSuitData, INCOMING_DATA_BUFFER_SIZE), [this]
		(const auto& error, auto bytes_transferred) {
			if (!error) {
				//We won't necessarily have the same pings for each hardware model. Two ways to go about this:
				//If we share significant logic between suit models, it might make sense to fix this issue here, and have each BoostSerialAdapter
				//know what suit it is connected to, how to communicate properly with it, etc.
				//If not, then maybe we end up making multiple plugins, one for mark2, one for mark3. Right now it seems like they are pretty similar so we can
				//getaway with things like IsPingPacket working for both a mark1 and a mark2. 

				if (!IsPingPacket(m_tempSuitData, bytes_transferred)) {
					//if it's not a ping packet, put it into the data stream. Don't want pings cluttering stuff up.
					m_filteredSuitData.push(m_tempSuitData, bytes_transferred);
					std::fill(m_tempSuitData, m_tempSuitData + INCOMING_DATA_BUFFER_SIZE, 0);
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





Buffer& BoostSerialAdapter::GetDataStream()
{
	return m_filteredSuitData;
}

bool BoostSerialAdapter::IsConnected() const
{
	return  !m_isReconnecting &&  this->m_port && this->m_port->is_open();
}

void BoostSerialAdapter::SetConnectionMonitor(std::shared_ptr<KeepaliveMonitor> monitor)
{
	m_keepaliveMonitor = monitor;	
	m_keepaliveMonitor->OnDisconnect([this]() { beginReconnectionProcess(); });

}


BoostSerialAdapter::~BoostSerialAdapter()
{
	Disconnect();
}








