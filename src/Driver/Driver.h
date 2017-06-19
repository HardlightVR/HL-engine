#pragma once
#include "DriverMessenger.h"
#include "HardwareInterface.h"
#include "FirmwareInterface.h"
#include "Encoder.h"
#include "SharedCommunication/ScheduledEvent.h"
#include "ImuConsumer.h"

#include "PluginManager.h"
#include "RegionRegistry.h"

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

	NullSpace::SharedMemory::TrackingUpdate m_cachedTracking;
	
	PluginManager m_pluginManager;   //order dependency (1)
	RegionRegistry m_regionRegistry; //order dependency (2)
	
	void handleTracking();
};