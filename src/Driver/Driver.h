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
	bool StartThread();
	bool Shutdown();
	void DrawDiagnostics(uint32_t id);
	int EnumeratePlugins(hvr_plugin_list * outPlugins);

	int GetPluginInfo(hvr_plugin_id id, hvr_plugin_info* outInfo);
	void GetCurrentSuitState(int* outState, uint64_t* outError);
private:
	IoService m_ioService;
	boost::asio::io_service& m_io;
	DriverMessenger m_messenger;


	PluginManager m_pluginManager;

	DeviceContainer m_devices;
	EventDispatcher m_eventDispatcher;


	HardwareCoordinator m_coordinator;


	ScheduledEvent m_hapticsPull;

	void handleHaptics();

	NullSpace::SharedMemory::TrackingUpdate m_cachedTracking;


	hvr_diagnostics_ui m_renderingApi;

	//todo: this is a hack for manufacturing verification of suits
	int m_currentSuitState;

public:
	void ProvideRenderingApi(hvr_diagnostics_ui * api);
};