#pragma once
#include "ICommunicationAdapter.h"
#include "ByteQueue.h"
#include "StdAfx.h"
#include "Serial.h"
#include "enumser.h"
#include <memory>
#include <string>
#include "Synchronizer.h"
class SerialAdapter : public virtual ICommunicationAdapter {
public:
	SerialAdapter();
	~SerialAdapter();
	bool Connect() override;
	void Disconnect() override;
	void Write(uint8_t bytes[], std::size_t length) override;
	void Read() override;
	bool Connect(std::string name);
	std::shared_ptr<CircularBuffer> GetDataStream() override;
private:
	std::unique_ptr<Serial> port;
	bool autoConnectPort();
	bool createPort(std::string name);
	std::shared_ptr<CircularBuffer> suitDataStream;

};