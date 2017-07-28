#pragma once
#include "PluginAPI.h"

namespace nsvr {
	namespace pevents {
		class device_event;
	}
}

class PluginApiRegistry;
class PluginEventHandler;

// This is the entrypoint that all plugin functions call into once they are configured.
// It simply reroutes to the appropriate components in the core.
class CoreFacade {
public:
	CoreFacade(PluginApiRegistry& apiRegistry, PluginEventHandler& eventHandler);


	void RaisePluginEvent(const nsvr::pevents::device_event& event);


	void RegisterPluginApi(nsvr_plugin_sampling_api* api);
	void RegisterPluginApi(nsvr_plugin_playback_api* api);
	void RegisterPluginApi(nsvr_plugin_preset_api* api);
	void RegisterPluginApi(nsvr_plugin_buffer_api* api);

	
private:
	PluginApiRegistry& m_apiRegistry;
	PluginEventHandler& m_eventHandler;
	
};