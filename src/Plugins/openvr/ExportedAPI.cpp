#include "stdafx.h"
#include "PluginAPI.h"
#include "OpenVRWrapper.h"

NSVR_PLUGIN_RETURN(int) nsvr_plugin_register(nsvr_plugin_api* api) {
	api->init = [](nsvr_plugin** plugin) {
		*plugin = AS_TYPE(nsvr_plugin, new OpenVRWrapper());
		return 1;
	};

	api->free = [](nsvr_plugin* plugin) {
		delete AS_TYPE(OpenVRWrapper, plugin);
		return 1;
	};

	api->configure = [](nsvr_plugin* plugin, nsvr_core* core) {
		AS_TYPE(OpenVRWrapper, plugin)->Configure(core);
		return 1;
	};

	return 1;
}