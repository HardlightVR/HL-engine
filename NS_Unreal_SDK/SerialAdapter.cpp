#include "StdAfx.h"
#include "SerialAdapter.h"
#include <iostream>

SerialAdapter::SerialAdapter() {
	//this->suitDataStream = ByteQueue();
	this->port = NULL;
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

bool SerialAdapter::createPort(std::string name) {
	this->port = std::unique_ptr<Serial>(new Serial(&name[0]));
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

void SerialAdapter::Write(char stuff[]) {
	if (this->port && this->port->IsConnected()) {
		if (!port->WriteData(stuff, sizeof(stuff))) {
			std::cout << "Stuff failed to write\n";
		}
	} 
}
void SerialAdapter::Read() {

}