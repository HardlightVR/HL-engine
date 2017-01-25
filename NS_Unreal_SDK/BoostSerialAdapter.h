#pragma once
#include "ICommunicationAdapter.h"
#include "SuitHardwareInterface.h"
#include <boost\lockfree\spsc_queue.hpp>

const int INCOMING_DATA_BUFFER_SIZE = 128;
class BoostSerialAdapter : public std::enable_shared_from_this<BoostSerialAdapter>, public virtual ICommunicationAdapter
{
public:
	bool Connect() override;
	void Disconnect() override;
	void Write(std::shared_ptr<uint8_t*> bytes, std::size_t length, std::function<void(const boost::system::error_code&, std::size_t)> cb) override;
	void Write(std::shared_ptr<uint8_t*> bytes, std::size_t length) override;
	void BeginRead() override;
	bool Connect(std::string name);
	std::shared_ptr<Buffer> GetDataStream() override;
	bool IsConnected() const override;
	BoostSerialAdapter(std::shared_ptr<IoService> io);
	~BoostSerialAdapter();
	bool NeedsReset() const override {
		return _needsReset;
	}
	void DoReset() override;

private:
	
	std::shared_ptr<IoService> _ioService;
	std::shared_ptr<boost::asio::io_service> _io;
	std::unique_ptr<boost::asio::serial_port> port;
	
	bool autoConnectPort();
	bool createPort(std::string name);
	void copy_data_to_circularbuff(std::size_t length);

	uint8_t _data[INCOMING_DATA_BUFFER_SIZE];
	std::shared_ptr<Buffer> suitDataStream;
	void doKeepAlivePing();

	void doSuitRead();
	void suitReadCancel(boost::system::error_code ec);
	void read_handler(boost::system::error_code ec, std::size_t length);

	void write_handler(boost::system::error_code ec, std::size_t length);
	boost::asio::deadline_timer _keepaliveTimer;
	boost::posix_time::milliseconds _keepaliveTimeout = boost::posix_time::milliseconds(20); //was 20
	std::chrono::milliseconds _initialConnectTimeout = std::chrono::milliseconds(100); //was 100
	boost::posix_time::milliseconds _pingTimeout = boost::posix_time::milliseconds(150); //was 250
	void reconnectSuit();
	long long _pingTime;
	boost::asio::deadline_timer _sendPingTimer;
	bool doHandshake( std::string portname);

	std::mutex _resetMutex;
	bool _needsReset;
	bool _isResetting = false;
	int _badPingCount = 0;
	
};

