#include "StdAfx.h"
#include "SerialAdapter.h"
#include <iostream>

SerialAdapter::SerialAdapter():suitDataStream(std::make_shared<CircularBuffer>(2048)){
	//this->suitDataStream = ByteQueue();
	this->port = nullptr;
}

SerialAdapter::~SerialAdapter() {

}

bool SerialAdapter::Connect() {
	return this->autoConnectPort();
}

bool SerialAdapter::autoConnectPort() {
	CEnumerateSerial::CPortsArray ports;
	CEnumerateSerial::CNamesArray names;
	if (!CEnumerateSerial::UsingQueryDosDevice(ports)) {
		std::cout << "No ports available." << "\n";
		return false;
	}

	for (std::size_t i = 0; i < ports.size(); ++i) {
		std::string strname = "COM" + std::to_string(ports[i]);
		if (this->createPort(strname)) {
			char ping[] = { 0x24, 0x02, 0x02, 0x07, 0xFF, 0xFF, 0x0A };
			this->port->WriteData(ping, 7);
			char result[1];
			if (this->port->ReadData(result, 1) > 0) {
				break;
			}
			
			
		}
	}
	if (this->port) {
		return this->port->IsConnected();
	}
	else {
		return false;
	}
}

bool SerialAdapter::Connect(std::string name) {
	if (name.find("\\\\.\\") != std::string::npos) {
		name = "\\\\.\\" + name;
	}
	return this->createPort(&name[0]);
}

std::shared_ptr<CircularBuffer> SerialAdapter::GetDataStream()
{
	return suitDataStream;
}

bool SerialAdapter::createPort(std::string name) {
	this->port = std::make_unique<Serial>(&name[0]);
	if (port->IsConnected()) {
		return true;
	}
	else {
		std::cout << "Tried to use " << name << " but couldn't open it." << "\n";
		return false;
	}
}

void SerialAdapter::Disconnect() {

}

void SerialAdapter::Write(uint8_t* stuff, std::size_t length) {
	if (this->port && this->port->IsConnected()) {
		char *chars = reinterpret_cast<char*>(stuff);

		if (!port->WriteData(chars, length)) {
			std::cout << "Stuff failed to write\n";
		}
	} 
}
void SerialAdapter::Read() {
	char bytesRead[1024];
	int actualRead = this->port->ReadData(bytesRead, 1024);
	if (actualRead > -1)
	{
		for (int i = 0; i < actualRead; ++i)
		{
			suitDataStream->push_back(bytesRead[i]);
		}
	}
}