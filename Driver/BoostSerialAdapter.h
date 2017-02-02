#pragma once
#include "ICommunicationAdapter.h"
#include <boost\lockfree\spsc_queue.hpp>
#include "IoService.h"
#include "KeepaliveMonitor.h"

const int INCOMING_DATA_BUFFER_SIZE = 128;

class BoostSerialAdapter : public std::enable_shared_from_this<BoostSerialAdapter>, public virtual ICommunicationAdapter
{
public:

	BoostSerialAdapter(std::shared_ptr<IoService> ioService);
	~BoostSerialAdapter();

	//Connect to the hardware through serial port
	bool Connect() override;
	//Disconnect from hardware by closing the port
	void Disconnect() override;
	//Write bytes to the port, custom callback receives error codes and size written
	void Write(std::shared_ptr<uint8_t*> bytes, std::size_t length, std::function<void(const boost::system::error_code&, std::size_t)> cb) override;
	//Write bytes to the port, no callback
	void Write(std::shared_ptr<uint8_t*> bytes, std::size_t length) override;

	//Retrieve the base stream that the adapter is working on
	std::shared_ptr<Buffer> GetDataStream() override;

	//Check if the adapter is connected to the hardware
	bool IsConnected() const override;



private:

	std::shared_ptr<IoService> _ioService;
	boost::asio::io_service& _io;

	std::unique_ptr<boost::asio::serial_port> port;

	void handleIoResetCallback();
	void beginRead();

	void copyDataToBuffer(std::size_t length);

	uint8_t _data[INCOMING_DATA_BUFFER_SIZE];
	std::shared_ptr<Buffer> suitDataStream;

	void doSuitRead();
	void read_handler(boost::system::error_code ec, std::size_t length);

	boost::posix_time::milliseconds _initialConnectTimeout;
	void reconnectSuit();

	bool _isResetting = false;

	boost::asio::deadline_timer _resetIoTimer;
	boost::posix_time::milliseconds _resetIoTimeout;
	KeepaliveMonitor _monitor;

	void testAllAsync();
	void testOne(std::vector<std::string> portNames);

	boost::asio::deadline_timer _cancelIoTimer;
};

