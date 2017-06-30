#include "stdafx.h"
#include "PluginAPI.h"
#include "PluginInstance.h"



NSVR_CORE_RETURN(int) NSVR_Configuration_GetCallback(NSVR_Configuration * config, const char * name, void ** outCallback, NSVR_Core_Ctx** context)
{
	if (config->Callbacks.find(name) != config->Callbacks.end()) {
		const auto& cb = config->Callbacks.at(name);
		*outCallback = cb.callback;
		*context = cb.context;
		return 1;
	}
	
	return -1;

}







