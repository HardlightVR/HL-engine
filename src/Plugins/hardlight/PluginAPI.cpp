#include "stdafx.h"
#include "PluginAPI.h"
#include "events/brieftaxel.h"
#include "events/LastingTaxel.h"
#include "Locator.h"
#include "EnumTranslator.h"

#include "HardlightPlugin.h"
#include <iostream>



NSVR_PLUGIN_RETURN(int) NSVR_Init(NSVR_Plugin ** pluginPtr)
{
	Locator::initialize();
	Locator::provide(new EnumTranslator());

	*pluginPtr = AS_TYPE(NSVR_Plugin, new HardlightPlugin());

	return 1;
}


NSVR_PLUGIN_RETURN(int) NSVR_Configure(NSVR_Plugin * pluginPtr, NSVR_Configuration * config)
{
	return AS_TYPE(HardlightPlugin, pluginPtr)->Configure(config);
}




NSVR_PLUGIN_RETURN(int) NSVR_Free(NSVR_Plugin ** ptr)
{
	delete AS_TYPE(HardlightPlugin, *ptr);
	*ptr = nullptr;
	return 1;
}





