#pragma once

#include "NSDriverApi.h"
#include "DriverMessenger.h"
#include "EventDispatcher.h"
#include "SharedCommunication/ScheduledEvent.h"
#include "HardwareCoordinator.h"
#include "PluginManager.h"

#include "DeviceContainer.h"
#include "CurveEngine.h"
class IoService;

class Driver {

public:
	
	Driver();
	~Driver();
	bool StartThread();
	bool Shutdown();
	void DrawDiagnostics(uint32_t id);
	int EnumeratePlugins(hvr_plugin_list * outPlugins);

	int GetPluginInfo(hvr_plugin_id id, hvr_plugin_info* outInfo);
	int CreateDevice(uint32_t device_id, hvr_device_tracking_datasource fn);
private:
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

	hvr_diagnostics_ui m_renderingApi;

public:
	void ProvideRenderingApi(hvr_diagnostics_ui * api);
};