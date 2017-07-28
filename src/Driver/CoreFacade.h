#pragma once
#include "PluginAPI.h"

namespace nsvr {
	namespace pevents {
		class device_event;
	}
}

class PluginCapabilities;
class PluginEventHandler;

// This is the entrypoint that all plugin functions call into once they are configured.
// It simply reroutes to the appropriate components in the core.
class CoreFacade {
public:
	CoreFacade(PluginCapabilities& apiRegistry, PluginEventHandler& eventHandler);


	void RaisePluginEvent(const nsvr::pevents::device_event& event);

	
	template<typename InternalApi, typename ExternalApi>
	void RegisterPluginApi(ExternalApi* api);
private:
	PluginCapabilities& m_apiRegistry;
	PluginEventHandler& m_eventHandler;
	
};

template<typename InternalApi, typename ExternalApi>
inline void CoreFacade::RegisterPluginApi(ExternalApi * api)
{
	m_apiRegistry.Register<InternalApi>(api);
}
