#include "stdafx.h"
#include "PluginAPI.h"
#include "RegionRegistry.h"
NSVR_CORE_RETURN(int) NSVR_Core_RegisterNode(NSVR_Core_t * core, NSVR_RegParams params)
{
	if (core == nullptr) {
		return -1;
	}

	if (params.Interface == nullptr || params.Provider == nullptr || params.Region == nullptr) {
		return -1;
	}

	return AS_TYPE(PluginInstance, core)->RegisterInterface(params);

}