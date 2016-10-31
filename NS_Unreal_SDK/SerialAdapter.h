#pragma once
#include "ICommunicationAdapter.h"
#include "Serial.h"
#include <memory>
#include "Synchronizer.h"
class SerialAdapter : public virtual ICommunicationAdapter {
public:
	SerialAdapter();
	~SerialAdapter();
	bool Connect() override;
	void Disconnect() override;
	void Write(uint8_t bytes[], std::size_t length) override;
	void BeginRead() override;
	bool Connect(std::string name);
	std::shared_ptr<Buffer> GetDataStream() override;
private:
	std::unique_ptr<Serial> port;
	bool autoConnectPort();
	bool createPort(std::string name);
	std::shared_ptr<Buffer> suitDataStream;

};