#pragma once
#include <memory>
#include "ICommunicationAdapter.h"
#include "boost\asio.hpp"
#include "boost\thread.hpp"
#include "handler_allocator.h"
#include <boost\asio\deadline_timer.hpp>
#include <boost\asio\use_future.hpp>
#include "SuitHardwareInterface.h"
#include "IoService.h"
#include <mutex>

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
class BoostSerialAdapter : public std::enable_shared_from_this<BoostSerialAdapter>, public virtual ICommunicationAdapter
{
public:
	bool Connect() override;
	void Disconnect() override;
	void Write(uint8_t bytes[], std::size_t length) override;
	void BeginRead() override;
	bool Connect(std::string name);
	std::shared_ptr<CircularBuffer> GetDataStream() override;
	bool IsConnected() const override;
	BoostSerialAdapter(std::shared_ptr<IoService> io);
	~BoostSerialAdapter();
	bool NeedsReset() const override {
		return _needsReset;
	}
	void DoReset() override;

private:
	handler_allocator _allocator;
	std::shared_ptr<IoService> _ioService;
	std::shared_ptr<boost::asio::io_service> _io;
	std::unique_ptr<boost::asio::serial_port> port;
	
	bool autoConnectPort();
	bool createPort(std::string name);
	void copy_data_to_circularbuff(std::size_t length);

	uint8_t _data[64];
	std::shared_ptr<CircularBuffer> suitDataStream;
	void doKeepAlivePing();

	void doSuitRead();
	void suitReadCancel(boost::system::error_code ec);
	void read_handler(boost::system::error_code ec, std::size_t length);
	void write_handler(boost::system::error_code ec, std::size_t length);
	boost::asio::deadline_timer _keepaliveTimer;
	boost::posix_time::milliseconds _keepaliveInterval = boost::posix_time::milliseconds(100);
	std::chrono::milliseconds _initialConnectTimeout = std::chrono::milliseconds(50);
	void reconnectSuit();
	long long _pingTime;

	bool doHandshake( std::string portname);

	std::mutex _resetMutex;
	bool _needsReset;
	bool _isResetting = false;
	
};

