#include "stdafx.h"
#include "PluginAPI.h"
#include "OpenVRWrapper.h"

NSVR_PLUGIN_RETURN(int) NSVR_Init(NSVR_Plugin** plugin) {
	*plugin = AS_TYPE(NSVR_Plugin, new OpenVRWrapper());
	return 1;
}

NSVR_PLUGIN_RETURN(int) NSVR_Free(NSVR_Plugin** plugin) {
	delete AS_TYPE(OpenVRWrapper, *plugin);
	*plugin = nullptr;
	return 1;
}

NSVR_PLUGIN_RETURN(int) NSVR_Configure(NSVR_Plugin* pluginPtr, nsvr_core_ctx* core) {
	AS_TYPE(OpenVRWrapper, pluginPtr)->Configure(core);
	return 1;
}