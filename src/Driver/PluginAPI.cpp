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



NSVR_CORE_RETURN(int) NSVR_Core_RegisterNode2(
	NSVR_Core * core, 
	NSVR_Consumer_Handler_t callback, 
	const char * iface, 
	const char* region,
	void * client_data
)
{

	return AS_TYPE(PluginInstance, core)->RegisterInterface2(callback, iface, region, client_data);
}