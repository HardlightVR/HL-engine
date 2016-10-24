#pragma once
#include <memory>
#include "ICommunicationAdapter.h"
#include "boost\asio.hpp"
#include "boost\thread.hpp"
class BoostSerialAdapter : public std::enable_shared_from_this<BoostSerialAdapter>, public virtual ICommunicationAdapter
{
public:
	bool Connect() override;
	void Disconnect() override;
	void Write(uint8_t bytes[], std::size_t length) override;
	void Read() override;
	bool Connect(std::string name);
	std::shared_ptr<CircularBuffer> GetDataStream() override;
	BoostSerialAdapter(std::shared_ptr<boost::asio::io_service>);
	~BoostSerialAdapter();
private:
	std::shared_ptr<boost::asio::io_service> _io;
	std::unique_ptr<boost::asio::serial_port> port;
	bool autoConnectPort();
	bool createPort(std::string name);
	uint8_t _data[64];
	void copy_data_to_circularbuff(std::size_t length);
	std::shared_ptr<CircularBuffer> suitDataStream;
};

