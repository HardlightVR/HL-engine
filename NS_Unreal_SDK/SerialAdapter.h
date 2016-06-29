#pragma once
#include "ICommunicationAdapter.h"
#include "ByteQueue.h"
#include "StdAfx.h"
#include "Serial.h"
#include "enumser.h"
#include <memory>
#include <string>
class SerialAdapter : public virtual ICommunicationAdapter {
public:
	SerialAdapter();
	~SerialAdapter();
	virtual bool Connect();
	virtual void Disconnect();
	virtual void Write(uint8_t bytes[], std::size_t length);
	virtual void Read();
	bool Connect(std::string name);
	ByteQueue suitDataStream;
private:
	std::unique_ptr<Serial> port;
	bool autoConnectPort();
	bool createPort(std::string name);
};