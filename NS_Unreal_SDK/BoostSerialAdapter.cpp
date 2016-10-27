#include "BoostSerialAdapter.h"
#include "enumser.h"
#include <iostream>
#include "BlockingReader.h"

bool BoostSerialAdapter::Connect()
{
	return this->autoConnectPort();

}

void BoostSerialAdapter::Disconnect()
{
}

void BoostSerialAdapter::Write(uint8_t bytes[], std::size_t length)
{
	if (this->port && this->port->is_open()) {
		char *chars = reinterpret_cast<char*>(bytes);
		this->port->async_write_some(boost::asio::buffer(bytes, length), 
			[&](const boost::system::error_code& error, std::size_t bytes_transferred) {
				if (error) { 
					std::cout << "Encoutered error writing to port (disconnecting): " << error.message() << "\n";
					//this->port->close();
				} else { 
				//	std::cout << "WROTE TO SUIT" << '\n';
				} 
		});
	}
}
void BoostSerialAdapter::read_handler(boost::system::error_code ec, std::size_t length) {
	if (!ec && length > 0) {
		_readSuitTimer.cancel();
		std::cout << "Got data from suit!" << '\n';
		this->copy_data_to_circularbuff(length);
		doSuitRead();

	}
	else {
		std::cout << "Error reading bytes! " <<ec.message()  << std::endl;
	}
}

void BoostSerialAdapter::doSuitRead()
{
	if ( this->port->is_open()) {

		this->port->async_read_some(boost::asio::buffer(_data, 64),
			boost::bind(&BoostSerialAdapter::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		//_readSuitTimer.expires_from_now(_readSuitTimeout);
		//_readSuitTimer.async_wait(boost::bind(&BoostSerialAdapter::suitReadCancel,
		//	this, boost::asio::placeholders::error));
	}
}

void BoostSerialAdapter::suitReadCancel(boost::system::error_code ec)
{
	if (ec) {
		return;
	}
	std::cout << "Timed out! Canceling all serial operations!" << '\n';
	this->port->cancel();
	
}

void BoostSerialAdapter::Read()
{
	std::cout << "Called read" << '\n';
	doSuitRead();
	
}
void BoostSerialAdapter::copy_data_to_circularbuff(std::size_t length) {
	for (std::size_t i = 0; i < length; ++i) {
		suitDataStream->push_front(_data[i]);
	}

	std::fill(_data,_data+64, 0);
}

void BoostSerialAdapter::doKeepAlivePing(const boost::system::error_code& ec)
{
	_hardware->PingSuit();
	//Schedule another ping for 10ms
	//_keepaliveTimer.expires_at(_keepaliveTimer.expires_at() + _keepaliveInterval);
	_keepaliveTimer.expires_from_now(_keepaliveInterval);
	_keepaliveTimer.async_wait(boost::bind(&BoostSerialAdapter::doKeepAlivePing, this, boost::asio::placeholders::error));

	_readSuitTimer.expires_from_now(_readSuitTimeout);
	_readSuitTimer.async_wait(boost::bind(&BoostSerialAdapter::suitReadCancel,
	this, boost::asio::placeholders::error));
}

bool BoostSerialAdapter::Connect(std::string name)
{
	return false;
}

std::shared_ptr<CircularBuffer> BoostSerialAdapter::GetDataStream()
{
	return this->suitDataStream;
}

bool BoostSerialAdapter::IsConnected() const
{
	return port->is_open();
}



BoostSerialAdapter::BoostSerialAdapter(std::shared_ptr<boost::asio::io_service> io, std::shared_ptr<SuitHardwareInterface> hardware):
	suitDataStream(std::make_shared<CircularBuffer>(4096)), port(nullptr), _io(io), _readSuitTimer(*io, _readSuitTimeout),_hardware(hardware),
	_keepaliveTimer(*io, _keepaliveInterval)

{
	std::fill(_data, _data + 64, 0);

}


BoostSerialAdapter::~BoostSerialAdapter()
{
}

bool BoostSerialAdapter::autoConnectPort()
{
	CEnumerateSerial::CPortsArray ports;
	CEnumerateSerial::CNamesArray names;
	if (!CEnumerateSerial::UsingQueryDosDevice(ports)) {
		std::cout << "No ports available." << "\n";
		return false;
	}
	bool foundPort = false;
	for (std::size_t i = 0; i < ports.size(); ++i) {
		std::string strname = "COM" + std::to_string(ports[i]);
		if (this->createPort(strname)) {
			char ping[] = { 0x24, 0x02, 0x02, 0x07, 0xFF, 0xFF, 0x0A };
			boost::asio::write(*(this->port), boost::asio::buffer(ping, 7));
			blocking_reader blocker(*this->port, 10);
			char c;
			if (blocker.read_char(c)) {
				foundPort = true;
				break;
			}


		}
	}
	if (foundPort && this->port) {
		//Ping the suit in 10ms or so
		_keepaliveTimer.expires_from_now(this->_keepaliveInterval);
		_keepaliveTimer.async_wait(boost::bind(&BoostSerialAdapter::doKeepAlivePing, this, boost::asio::placeholders::error));
		//return if open instead of true? Or already determined by the blocker.read_char
		return true;
	}
	else {
		this->port->close();
		return false;
	}
}

bool BoostSerialAdapter::createPort(std::string name)
{
	this->port = std::unique_ptr<boost::asio::serial_port>(new boost::asio::serial_port(*_io));
	this->port->open(name);
	if (port->is_open()) {
		return true;
	}
	else {
		std::cout << "Tried to use " << name << " but couldn't open it." << "\n";
		return false;
	}
}

