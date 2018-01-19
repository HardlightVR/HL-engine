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
	void GetCurrentSuitState(hvr_suitstate* outState);
private:
	IoService m_ioService;
	boost::asio::io_service& m_io;
	DriverMessenger m_messenger;


	PluginManager m_pluginManager;

	DeviceContainer m_devices;
	EventDispatcher m_eventDispatcher;


	HardwareCoordinator m_coordinator;


	ScheduledEvent m_hapticsPull;
	ScheduledEvent m_statusPush;

	void handleHaptics();
	void handleStatus();

	NullSpace::SharedMemory::TrackingUpdate m_cachedTracking;


	hvr_diagnostics_ui m_renderingApi;

	//todo: this is a hack for manufacturing verification of suits
	hvr_suitstate m_currentSuitState;

public:
	void ProvideRenderingApi(hvr_diagnostics_ui * api);
};