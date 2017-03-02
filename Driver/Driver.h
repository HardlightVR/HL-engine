#pragma once
#include "DriverMessenger.h"
#include "ClientMessenger.h"
#include "HardwareInterface.h"
#include "FirmwareInterface.h"
#include "Encoder.h"
#include "ScheduledEvent.h"
#include "ImuConsumer.h"
class IoService;

class Driver {
public:
	Driver();
	~Driver();
	bool StartThread();
	bool Shutdown();
private:
	std::atomic<bool> m_running;

	std::shared_ptr<IoService> _io;
	DriverMessenger m_messenger;
	HardwareInterface m_hardware;
	Encoder _encoder;

	
	ImuConsumer m_imus;

	ScheduledEvent m_hapticsPull;
	ScheduledEvent m_statusPush;
	ScheduledEvent m_commandPull;
	ScheduledEvent m_trackingPush;

	void handleHaptics();
	void handleStatus();
	void handleCommands();

	

	
};