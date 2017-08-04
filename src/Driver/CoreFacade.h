#pragma once
#include "PluginAPI.h"

namespace nsvr {
	namespace pevents {
		class device_event;
	}
}

class PluginApis;
class PluginEventHandler;

// This is the entrypoint that all plugin functions call into once they are configured.
// It simply reroutes to the appropriate components.
class CoreFacade {
public:
	CoreFacade(PluginApis& apiRegistry, PluginEventHandler& eventHandler);


	void RaisePluginEvent(nsvr_device_event_type type, uint64_t id);

	
	template<typename InternalApi, typename ExternalApi>
	void RegisterPluginApi(ExternalApi* api);
private:
	PluginApis& m_pluginCapabilities;
	PluginEventHandler& m_eventHandler;
	
};

template<typename InternalApi, typename ExternalApi>
inline void CoreFacade::RegisterPluginApi(ExternalApi * api)
{
	m_pluginCapabilities.Register<InternalApi>(api);
}
