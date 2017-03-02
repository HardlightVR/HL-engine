#include "StdAfx.h"
#include "BoostSerialAdapter.h"
#include <iostream>
#include <future>
#include <boost/asio/use_future.hpp>
#include "enumser.h"
#include "Locator.h"
BoostSerialAdapter::BoostSerialAdapter(std::shared_ptr<IoService> ioService) :
	suitDataStream(std::make_shared<Buffer>(4096)), port(nullptr), _ioService(ioService),
	_io(ioService->GetIOService()),
	_resetIoTimeout(boost::posix_time::milliseconds(500)),
	_initialConnectTimeout(boost::posix_time::milliseconds(300)),
	_resetIoTimer(_io),
	_monitor(ioService->GetIOService(), port),
	_cancelIoTimer(_io)

{
	std::fill(_data, _data + INCOMING_DATA_BUFFER_SIZE, 0);
	_monitor.SetDisconnectHandler(std::bind(&BoostSerialAdapter::reconnectSuit, this));
}


void BoostSerialAdapter::Connect()
{
	

	this->testAllAsync();
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
//	Locator::Logger().Log("Adapter", "Attempting to auto-reconnect..", LogLevel::Info);

	this->testAllAsync();

}
void BoostSerialAdapter::reconnectSuit() {
	Locator::Logger().Log("Adapter", "Reconnecting..");
	_isResetting = true;
	_ioService->RestartIOService(std::bind(&BoostSerialAdapter::handleIoResetCallback, this));
	
	
}
void BoostSerialAdapter::beginRead()
{
	_isResetting = false;
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


void BoostSerialAdapter::testAllAsync() {
	CEnumerateSerial::CPortsArray ports;
	CEnumerateSerial::CNamesArray names;
	if (!CEnumerateSerial::UsingQueryDosDevice(ports)) {
		Locator::Logger().Log("Adapter", "No ports available on system. Check Device Manager for available devices.", LogLevel::Warning);
	}
	std::vector<std::string> portNames;
	for (std::size_t i = 0; i < ports.size(); ++i) {
		portNames.push_back("COM" + std::to_string(ports[i]));
	}
	
	_io.post([this, portNames]() {testOne(portNames); });
	

}
void BoostSerialAdapter::testOne(std::vector<std::string> portNames) {
	if (portNames.empty()) {
		//have tested everything, all we can do is go back and try everything again
		_io.post([this]() {testAllAsync(); });
		return;
	}

	if (port && port->is_open()) {
		try {
			port->close();
		}
		catch (const boost::system::system_error& ec) {
			std::cout << "Failed to close this port " << '\n';
		}
	}

	this->port = std::make_unique<boost::asio::serial_port>(_io);

	std::string portName = portNames.back();
	portNames.pop_back();

	try {
		port->open(portName);
		if (!port->is_open()) {
			_io.post(boost::bind(&BoostSerialAdapter::testOne, this, portNames));
		}
	}
	catch (boost::system::system_error& ec) {
		this->reconnectSuit();
		return;

	}


	auto pingData = std::make_shared<uint8_t*>(new uint8_t[7]{ 0x24, 0x02, 0x02, 0x07, 0xFF, 0xFF, 0x0A });
	this->port->async_write_some(boost::asio::buffer(*pingData, 7), [pingData](const boost::system::error_code, const std::size_t bytes_transferred) {});

	_cancelIoTimer.expires_from_now(_initialConnectTimeout);

	_cancelIoTimer.async_wait([this](const boost::system::error_code& ec) {
		if (ec) {
			std::cout << "From cancel timer: " << ec.message() << '\n';
		}
		else {
			if (port && port->is_open()) {
				try {
						port->close();
						std::cout << "Failed to connect to this port within 50 ms, closed it\n";
					
					

				}
				catch (const boost::system::system_error& ec) {
					std::cout << "Failed to connect to this port within 50 ms, and couldn't even close it..\n";

				}
			}
			else {
				std::cout << "Failed to connect to this port within 50 ms\n";

			}

		}
	});
	port->async_read_some(boost::asio::buffer(_data, INCOMING_DATA_BUFFER_SIZE),[this,portNames] 
	(const boost::system::error_code ec, const std::size_t bytes_transferred) {
		if (ec) {

			int num_canceled = _cancelIoTimer.cancel();
			assert(num_canceled == 0);
			std::cout << "I was canceled, moving on to the next\n";
			_io.post(boost::bind(&BoostSerialAdapter::testOne, this, portNames));


		}
		else {
			//connected

			int num_canceled = _cancelIoTimer.cancel();
		//	assert(bytes_transferred == 16);
			//assert(num_canceled==1);
			assert(port && port->is_open());
			std::cout << "recd back " << bytes_transferred << " bytes\n";
			_io.post(boost::bind(&BoostSerialAdapter::beginRead, this));
		}
	});
	
	
}




