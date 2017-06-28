#pragma once
#include "DriverMessenger.h"
#include "HardwareInterface.h"
#include "Encoder.h"
#include "SharedCommunication/ScheduledEvent.h"

#include "PluginManager.h"



class IoService;

class Driver {

public:
	
	Driver();
	~Driver();
	bool StartThread();
	bool Shutdown();
private:

	std::shared_ptr<IoService> m_io;
	DriverMessenger m_messenger;
	Encoder _encoder;

	

	ScheduledEvent m_hapticsPull;
	ScheduledEvent m_statusPush;
	ScheduledEvent m_commandPull;
	ScheduledEvent m_trackingPush;

	void handleHaptics();
	void handleStatus();
	void handleCommands();

	NullSpace::SharedMemory::TrackingUpdate m_cachedTracking;
	
	PluginManager m_pluginManager;   
	HardwareInterface m_hardware;

	void handleTracking();
};