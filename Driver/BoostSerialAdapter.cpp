#include "StdAfx.h"
#include "BoostSerialAdapter.h"
#include <iostream>
#include <future>
#include "enumser.h"
#include "Locator.h"
BoostSerialAdapter::BoostSerialAdapter(std::shared_ptr<IoService> ioService) :
	suitDataStream(std::make_shared<Buffer>(4096)), port(nullptr), _ioService(ioService),
	_io(ioService->GetIOService()),
	_resetIoTimeout(boost::posix_time::milliseconds(500)),
	_initialConnectTimeout(std::chrono::milliseconds(100)),
	_resetIoTimer(_io),
	_monitor(ioService->GetIOService(), port)

{
	std::fill(_data, _data + INCOMING_DATA_BUFFER_SIZE, 0);
	_monitor.SetDisconnectHandler(std::bind(&BoostSerialAdapter::reconnectSuit, this));
}


bool BoostSerialAdapter::Connect()
{
	if (this->autoConnectPort()) {
		this->beginRead();
		return true;
	}
	else {
		this->beginRead();
		return false;
	}

}

void BoostSerialAdapter::Disconnect()
{
	if (port->is_open()) {
		port->close();
	}
}
void BoostSerialAdapter::Write(std::shared_ptr<uint8_t*> bytes, std::size_t length, std::function<void(const boost::system::error_code&, std::size_t)> cb)
{
	if (this->port && this->port->is_open()) {
		this->port->async_write_some(boost::asio::buffer(*bytes, length), cb);
	}
}
void BoostSerialAdapter::Write(std::shared_ptr<uint8_t*> bytes, std::size_t length)
{
	if (this->port && this->port->is_open()) {
		this->port->async_write_some(boost::asio::buffer(*bytes, length),
			[bytes](const boost::system::error_code& error, std::size_t bytes_transferred) {
			if (error) {
				Locator::Logger().Log("Adapter", "Failed to write to suit!", LogLevel::Error);
			}
		});
	}
}
void BoostSerialAdapter::read_handler(boost::system::error_code ec, std::size_t length) {
	if (!ec && length > 3) {
		//todo: this is checking if it was a ping packet. Should be written better, maybe with a helper function
		//in the NS namespace to check that it's a ping packet. 
		if (_data[2] == 0x02) {
			_monitor.ReceivePing();
		}
		
		this->copyDataToBuffer(length);
		doSuitRead();
	}
	
	
}

void BoostSerialAdapter::doSuitRead()
{
	if (this->port && this->port->is_open()) {
		this->port->async_read_some(boost::asio::buffer(_data, INCOMING_DATA_BUFFER_SIZE),
			boost::bind(&BoostSerialAdapter::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}	
}







void BoostSerialAdapter::handleIoResetCallback() {
	Locator::Logger().Log("Adapter", "Attempting to auto-reconnect..", LogLevel::Info);

	if (this->autoConnectPort()) {
		this->beginRead();
		Locator::Logger().Log("Adapter", "Auto reconnected!", LogLevel::Info);
		_isResetting = false;
	}
	else {
		_resetIoTimer.expires_from_now(_resetIoTimeout);
		_resetIoTimer.async_wait(boost::bind(&BoostSerialAdapter::reconnectSuit, this));

	}

}
void BoostSerialAdapter::reconnectSuit() {

	_isResetting = true;
	_ioService->RestartIOService(std::bind(&BoostSerialAdapter::handleIoResetCallback, this));
	
	
}
void BoostSerialAdapter::beginRead()
{
	_monitor.BeginMonitoring();
	doSuitRead();

}
void BoostSerialAdapter::copyDataToBuffer(std::size_t length) {

	suitDataStream->push(_data, length);

	//zero the buffer
	std::fill(_data,_data + INCOMING_DATA_BUFFER_SIZE, 0);
}



std::shared_ptr<Buffer> BoostSerialAdapter::GetDataStream()
{
	return this->suitDataStream;
}

bool BoostSerialAdapter::IsConnected() const
{
	return  !_isResetting && this->port && this->port->is_open();
}




BoostSerialAdapter::~BoostSerialAdapter()
{
	Disconnect();
}

bool BoostSerialAdapter::autoConnectPort()
{

	CEnumerateSerial::CPortsArray ports;
	CEnumerateSerial::CNamesArray names;
	if (!CEnumerateSerial::UsingQueryDosDevice(ports)) {
		Locator::Logger().Log("Adapter", "No ports available on system. Check Device Manager for available devices.", LogLevel::Warning);
		return false;
	}
	for (std::size_t i = 0; i < ports.size(); ++i) {
		std::string strname = "COM" + std::to_string(ports[i]);
		if (this->doHandshake(strname)) {
			return true;
		}
	}
	return false;

}



bool BoostSerialAdapter::doHandshake(std::string portName) {

	//First, we attempt to create and connect to the port. 
	//Then, we send a short ping and see if we receive a response.

	if (this->openPort(portName)) {
		auto pingData = std::make_shared<uint8_t*>(new uint8_t[7]{ 0x24, 0x02, 0x02, 0x07, 0xFF, 0xFF, 0x0A });
		this->port->async_write_some(boost::asio::buffer(*pingData, 7), [pingData](const boost::system::error_code, const std::size_t bytes_transferred) {});
		//Don't want to deal with more async handlers here, so use a std::future to wait for a couple hundred millis
		//(suit takes about 30ms first ping)
		std::future<std::size_t> length = port->async_read_some(boost::asio::buffer(_data, INCOMING_DATA_BUFFER_SIZE), boost::asio::use_future);
		auto status = length.wait_for(_initialConnectTimeout);
		switch (status) {
		case std::future_status::ready:
			return true;
		case std::future_status::timeout:
			return false;
		case std::future_status::deferred:
			return false;
		default:
			return false;
		}
	}
	return false;
}

bool BoostSerialAdapter::openPort(std::string name)
{

	//Need to close old port if it is open
	
	if (this->port && this->port->is_open()) {
		try {
			this->port->close();
			if (this->port->is_open()) {
				Locator::Logger().Log("Adapter", "Major error, but possibly recoverable: After closing port, the port was still open. Talk to casey@nullspacevr.com", LogLevel::Warning);
			}
		}
		catch (boost::system::system_error& e) {
			Locator::Logger().Log("Adapter", "Failed to close a port", LogLevel::Error);
		}
		
	}

	this->port = std::make_unique<boost::asio::serial_port>(_io);

	try {
		this->port->open(name);
		return this->port->is_open();
	}
	catch (boost::system::system_error& e) {
		//This is the most likely case for failing when opening a port. If there is no device connected, we hit
		//this case.
		return false;
	}

}

