#pragma once

#include "runtime_include/NSDriverApi.h"
#include "DriverMessenger.h"
#include "EventDispatcher.h"
#include "SharedCommunication/ScheduledEvent.h"
#include "HardwareCoordinator.h"
#include "PluginManager.h"

#include "DeviceContainer.h"

#include "IoService.h"

class Driver {

public:
	
	Driver();
	~Driver();
	bool StartThread();
	bool Shutdown();
	void DrawDiagnostics(uint32_t id);
	int EnumeratePlugins(hvr_plugin_list * outPlugins);

	int GetPluginInfo(hvr_plugin_id id, hvr_plugin_info* outInfo);
private:
	IoService m_ioService;
	boost::asio::io_service& m_io;
	DriverMessenger m_messenger;


	PluginManager m_pluginManager;

	DeviceContainer m_devices;
	EventDispatcher m_eventDispatcher;


	HardwareCoordinator m_coordinator;


	ScheduledEvent m_curveEngineUpdate;

	ScheduledEvent m_hapticsPull;
	ScheduledEvent m_statusPush;
	ScheduledEvent m_commandPull;
	ScheduledEvent m_trackingPush;

	void handleHaptics();
	void handleStatus();
	void handleCommands();

	NullSpace::SharedMemory::TrackingUpdate m_cachedTracking;

	void handleTracking();

	hvr_diagnostics_ui m_renderingApi;

public:
	void ProvideRenderingApi(hvr_diagnostics_ui * api);
};