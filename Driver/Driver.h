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
	std::atomic<bool> m_running;
	std::thread _workThread;
	std::thread _messengerThread;
	void handleHaptics(const boost::system::error_code&);
	void handleStatus(const boost::system::error_code&);
	boost::asio::deadline_timer m_hapticsPollTimer;
	boost::posix_time::milliseconds m_hapticsPollInterval;

	boost::asio::deadline_timer m_statusPushTimer;
	boost::posix_time::milliseconds m_statusPushInterval;

	DriverMessenger m_messenger;
	HardwareInterface m_hardware;
	Encoder _encoder;

	PacketDispatcher m_dispatcher;

	void scheduleHapticsPoll();
	void scheduleStatusPush();
	
};