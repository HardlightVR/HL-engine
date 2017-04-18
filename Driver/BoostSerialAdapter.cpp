#include "StdAfx.h"
#include "BoostSerialAdapter.h"
#include <iostream>
#include <future>
#include <boost/asio/use_future.hpp>
#include "enumser.h"
#include "Locator.h"
#include "AsyncTimeout.h"
#include <boost\log\trivial.hpp>

uint8_t BoostSerialAdapter::m_pingData[7] = { 0x24, 0x02, 0x02, 0x07, 0xFF, 0xFF, 0x0A };

BoostSerialAdapter::BoostSerialAdapter(boost::asio::io_service& io) :
	m_outputSuitData(std::make_shared<Buffer>(4096)), 
	m_port(nullptr),
	m_io(io),
	m_suitReconnectionTimeout(boost::posix_time::milliseconds(50)),
	m_initialConnectTimeout(boost::posix_time::milliseconds(300)),
	m_suitReconnectionTimer(io),
	m_keepaliveMonitor(io, m_port)
{
	std::fill(m_data, m_data + INCOMING_DATA_BUFFER_SIZE, 0);
	m_keepaliveMonitor.SetDisconnectHandler([this]() { beginReconnectionProcess(); });
}

void BoostSerialAdapter::Connect()
{
	//When we first connect, we don't want any delays
	beginReconnectionProcess();
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
			if (error) {
				Locator::Logger().Log("Adapter", "Failed to write to suit!", LogLevel::Error);
			}
		});
	}
}


void BoostSerialAdapter::kickoffSuitReading()
{
	if (!this->m_port || !this->m_port->is_open()) {
		return;
	}

	this->m_port->async_read_some(boost::asio::buffer(m_data, INCOMING_DATA_BUFFER_SIZE), [this]
		(auto error, auto bytes_transferred) {
		
			if (!error) {
				
				if (!isPingPacket(m_data, bytes_transferred)) {
					//if it's not a ping packet, put it into the data stream. Don't want pings cluttering stuff up.
					m_outputSuitData->push(m_data, bytes_transferred);
					std::fill(m_data, m_data + INCOMING_DATA_BUFFER_SIZE, 0);
				}
				else {
					m_keepaliveMonitor.ReceivePing();
				}

				kickoffSuitReading();
			}	
		}
	);
	
}







void BoostSerialAdapter::beginReconnectionProcess() {
	BOOST_LOG_TRIVIAL(trace) << "[Adapter] Disconnected from suit  ";

	std::cout << "Disconnected..\n";
	_isResetting = true;
	scheduleImmediateSuitReconnect();
}
void BoostSerialAdapter::endReconnectionProcess()
{
	_isResetting = false;
	m_keepaliveMonitor.BeginMonitoring();
	kickoffSuitReading();
}



std::shared_ptr<Buffer> BoostSerialAdapter::GetDataStream()
{
	return this->m_outputSuitData;
}

bool BoostSerialAdapter::IsConnected() const
{
	return  !_isResetting &&  this->m_port && this->m_port->is_open();
}




BoostSerialAdapter::~BoostSerialAdapter()
{
	Disconnect();
}

/*
This method kicks off the connection to the serial ports.

It attempts to check all available ports, sending a ping to each. If it receives the magic response,
it will use that port. 
*/

void BoostSerialAdapter::testAllPorts(const boost::system::error_code& ec) {

	if (ec) {
		return;
	}
	//First, we retrieve all the available ports. This is windows-only. 
	CEnumerateSerial::CPortsArray ports;
	CEnumerateSerial::CNamesArray names;
	if (!CEnumerateSerial::UsingQueryDosDevice(ports)) {
		BOOST_LOG_TRIVIAL(info) << "[Adapter] No ports available on system. Check Device Manager for available devices.";

		Locator::Logger().Log("Adapter", "No ports available on system. Check Device Manager for available devices.", LogLevel::Warning);
	}

	//Then, we append "COM" to each of them.
	std::vector<std::string> portNames;
	for (std::size_t i = 0; i < ports.size(); ++i) {
		portNames.push_back("COM" + std::to_string(ports[i]));
		Locator::Logger().Log("Adapter", portNames.back());
	}
	
	//Finally, we post the testOnePort method to the IO service. This method will test each port
	//sequentially until it finds the correct port or runs out of ports, in which case it will
	//start over again after a short delay.
	m_io.post([this, portNames]() { testOnePort(portNames); });

}


//This is quite a large method. Not sure if splitting into two would 
//help with understanding it, but if so then do it!
//This method tries to connect to each port in the portNames list one after the other
void BoostSerialAdapter::testOnePort(std::vector<std::string> portNames) {

	//If the list of ports is empty, we've exhausted all our options.
	//Therefore, we must wait a bit and then try everything again
	if (portNames.empty()) {
		scheduleDelayedSuitReconnect();
		return;
	}

	//Call the port's destructor and reinstantiate it
	m_port = std::make_unique<boost::asio::serial_port>(m_io);

	//Grab the next portname and remove it from the vector
	std::string portName = portNames.back();
	portNames.pop_back();

	//Attempt to simply open up the serial port. If that doesn't work, try the next..
	if (!tryOpenPort(*m_port, portName)) {
		m_io.post([portNames, this] {testOnePort(portNames); }); 
		return;
	}

	//Okay, the port is open. Now send a ping to it.
	m_port->async_write_some(boost::asio::buffer(m_pingData, 7), [](auto ec, auto bytes_transferred) {});
	
	//Read from the port with a specified timeout (if the timeout is reached, we move on to the next) 
	auto timedReader = std::make_shared<AsyncTimeout>(m_io, boost::posix_time::millisec(m_initialConnectTimeout));
	
	timedReader->OnTimeout([&](){
		//if we end up timing out, we should close the port if it was opened
		if (m_port && m_port->is_open()) {
			try {
				m_port->close();
			}
			catch (const boost::system::system_error&) {
				//intentionally left blank
			}
		}
	});

	//Begin the timeout countdown
	timedReader->Go();
	

	m_port->async_read_some(boost::asio::buffer(m_data, INCOMING_DATA_BUFFER_SIZE), [this, portNames, timedReader, portName] 
		(auto ec, auto bytes_transferred) {
		
		//Stop the timed read from potentially expiring
		timedReader->Cancel();

		//Success case
		if (!ec) {
			assert(m_port && m_port->is_open());
			if (isPingPacket(m_data, bytes_transferred)) {
				BOOST_LOG_TRIVIAL(info) << "[Adapter] Connected to a suit on port " << portName;

				m_io.post([this] { endReconnectionProcess(); });
				return;
			}
		}

		//got some kind of error reading, or it wasn't a ping packet, so try the next
		m_io.post([portNames, this]() { testOnePort(portNames);});
	});
}

bool BoostSerialAdapter::tryOpenPort(boost::asio::serial_port& port, std::string portName)
{
	try {
		port.open(portName);
		if (!port.is_open()) {
			return false;
		}
	}
	catch (const boost::system::system_error&) {
	//	BOOST_LOG_TRIVIAL(trace) << "[Adapter] Got an exception when trying to open port:  " << ec.what();
		return false;
	}

	return true;
}

void BoostSerialAdapter::scheduleImmediateSuitReconnect()
{
	m_suitReconnectionTimer.expires_from_now(boost::posix_time::millisec(10));
	m_suitReconnectionTimer.async_wait([this](auto err) {testAllPorts(err); });
}

bool BoostSerialAdapter::isPingPacket(uint8_t* data, std::size_t length)
{
	if (length < 3) {
		return false;
	}

	return (
			data[0] == '$'
		 && data[1] == 0x02
		 && data[2] == 0x02
	);
		
}

void BoostSerialAdapter::scheduleDelayedSuitReconnect()
{

	m_suitReconnectionTimer.expires_from_now(m_suitReconnectionTimeout);
	m_suitReconnectionTimer.async_wait([this](auto err) {testAllPorts(err); });

}



