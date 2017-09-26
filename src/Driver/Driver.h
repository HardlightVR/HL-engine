#pragma once
#include "DriverMessenger.h"
#include "EventDispatcher.h"
#include "SharedCommunication/ScheduledEvent.h"
#include "HardwareCoordinator.h"
#include "PluginManager.h"

#include "DeviceContainer.h"
#include "CurveEngine.h"
#include "IdentificationService.h"
class IoService;

class Driver {

public:
	
	Driver();
	~Driver();
	bool StartThread();
	bool Shutdown();
private:
	IdentificationService m_idService;
	DeviceContainer m_devices;
	EventDispatcher m_eventDispatcher;

	std::shared_ptr<IoService> m_ioService;
	boost::asio::io_service& m_io;

	DriverMessenger m_messenger;


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
	void handleTracking();

	//CurveEngine m_curveEngine;


};