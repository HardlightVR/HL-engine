#include "stdafx.h"
#include "PluginAPI.h"
#include "OpenVRWrapper.h"

NSVR_PLUGIN_RETURN(int) NSVR_Plugin_Init(nsvr_plugin** plugin) {
	*plugin = AS_TYPE(nsvr_plugin, new OpenVRWrapper());
	return 1;
}

NSVR_PLUGIN_RETURN(int) NSVR_Plugin_Free(nsvr_plugin** plugin) {
	delete AS_TYPE(OpenVRWrapper, *plugin);
	*plugin = nullptr;
	return 1;
}

NSVR_PLUGIN_RETURN(int) NSVR_Plugin_Configure(nsvr_plugin* pluginPtr, nsvr_core* core) {
	AS_TYPE(OpenVRWrapper, pluginPtr)->Configure(core);
	return 1;
}