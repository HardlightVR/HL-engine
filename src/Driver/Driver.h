#pragma once
#include "DriverMessenger.h"
#include "HardwareInterface.h"
#include "Encoder.h"
#include "SharedCommunication/ScheduledEvent.h"
#include "HardwareCoordinator.h"
#include "PluginManager.h"


#include "CurveEngine.h"
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

	
	ScheduledEvent m_curveEngineUpdate;

	ScheduledEvent m_hapticsPull;
	ScheduledEvent m_statusPush;
	ScheduledEvent m_commandPull;
	ScheduledEvent m_trackingPush;

	void handleHaptics();
	void handleStatus();
	void handleCommands();

	NullSpace::SharedMemory::TrackingUpdate m_cachedTracking;
	HardwareCoordinator m_coordinator;

	PluginManager m_pluginManager;   
	HardwareInterface m_hardware;

	void handleTracking();
	//CurveEngine m_curveEngine;
};