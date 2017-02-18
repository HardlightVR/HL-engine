#pragma once
#include "DriverMessenger.h"
#include "ClientMessenger.h"
#include "HardwareInterface.h"
#include "FirmwareInterface.h"
#include "Encoder.h"
#include "PacketDispatcher.h"
class IoService;

class Driver {
public:
	Driver();
	~Driver();
	bool StartThread();
	bool Shutdown();
private:
	std::shared_ptr<IoService> _io;
	std::thread _clientThread;
	std::atomic<bool> _running;
	std::thread _workThread;
	std::thread _messengerThread;
	void handleHaptics(const boost::system::error_code&);
	
	boost::asio::deadline_timer m_hapticsPollTimer;
	boost::posix_time::milliseconds m_hapticsPollInterval;
	int counter = 1;
	DriverMessenger _messenger;
//	ClientMessenger _cMessenger;
	HardwareInterface _hardware;
	Encoder _encoder;

	PacketDispatcher m_dispatcher;

	void scheduleHapticsPoll();
	
};