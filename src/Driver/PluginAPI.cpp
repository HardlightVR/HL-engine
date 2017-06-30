#include "stdafx.h"
#include "PluginAPI.h"
#include "PluginInstance.h"



NSVR_CORE_RETURN(int) NSVR_Configuration_GetCallback(NSVR_Configuration * config, const char * name, void ** outCallback, NSVR_Core_Ctx** context)
{

	if (strcmp(name, "tracking") == 0) {
		
	
		return 1;
	}
	else if (strcmp(name, "status") == 0) {
		*outCallback = config->StatusCallback.callback;
		*context = config->StatusCallback.context;
		return 1;

	}
	else if (strcmp(name, "register-node") == 0) {
		*outCallback = config->RegisterNodeCallback.callback;
		*context = config->RegisterNodeCallback.context;
		return 1;

	}
	else {
		return -1;
	}
}







