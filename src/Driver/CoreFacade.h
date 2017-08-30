#pragma once
#include "PluginAPI.h"


class PluginApis;
class PluginEventSource;

// This is the entrypoint that all plugin functions call into once they are configured.
// It simply reroutes to the appropriate components.
class CoreFacade {
public:
	CoreFacade(PluginApis& apiRegistry, PluginEventSource& eventHandler);


	void RaisePluginEvent(nsvr_device_event_type type, nsvr_device_id id);

	
	template<typename InternalApi, typename ExternalApi>
	void RegisterPluginApi(ExternalApi* api);
private:
	PluginApis& m_pluginCapabilities;
	PluginEventSource& m_eventHandler;
	
};

template<typename InternalApi, typename ExternalApi>
inline void CoreFacade::RegisterPluginApi(ExternalApi * api)
{
	m_pluginCapabilities.Register<InternalApi>(api);
}
