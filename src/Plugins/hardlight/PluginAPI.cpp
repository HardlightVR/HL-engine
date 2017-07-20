#include "stdafx.h"
#include "PluginAPI.h"

#include "Locator.h"
#include "EnumTranslator.h"

#include "HardlightPlugin.h"
#include <iostream>





NSVR_PLUGIN_RETURN(int) NSVR_Plugin_Init(nsvr_plugin ** pluginPtr)
{
	Locator::initialize();
	Locator::provide(new EnumTranslator());

	*pluginPtr = AS_TYPE(nsvr_plugin, new HardlightPlugin());

	return 1;
}




NSVR_PLUGIN_RETURN(int) NSVR_Plugin_Configure(nsvr_plugin * pluginPtr, nsvr_core * core)
{
	return AS_TYPE(HardlightPlugin, pluginPtr)->Configure(core);
}





NSVR_PLUGIN_RETURN(int) NSVR_Plugin_Free(nsvr_plugin ** ptr)
{
	delete AS_TYPE(HardlightPlugin, *ptr);
	*ptr = nullptr;
	return 1;
}




