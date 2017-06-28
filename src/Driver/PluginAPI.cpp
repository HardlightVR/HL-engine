#include "stdafx.h"
#include "PluginAPI.h"
#include "RegionRegistry.h"



NSVR_CORE_RETURN(int) NSVR_Core_RegisterNode(
	NSVR_Core * core, 
	NSVR_Consumer_Handler_t callback, 
	const char * iface, 
	const char* region,
	void * client_data
)
{

	return AS_TYPE(PluginInstance, core)->RegisterInterface(callback, iface, region, client_data);
}