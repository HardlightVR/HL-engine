#include "stdafx.h"
#include "PluginAPI.h"
#include "PluginInstance.h"



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




NSVR_CORE_RETURN(int) NSVR_Core_Tracking_Submit(NSVR_Core* core, const char* region, const NSVR_Core_Quaternion* update)
{
	return AS_TYPE(PluginInstance, core)->UpdateTracking(region, update);
}


NSVR_CORE_RETURN(int) NSVR_Core_ConnectionStatus_Submit(NSVR_Core* core, bool isDeviceConnected)
{
	return AS_TYPE(PluginInstance, core)->UpdateDeviceStatus(isDeviceConnected);
}

