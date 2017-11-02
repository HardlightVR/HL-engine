#include "stdafx.h"
#include "PluginAPI.h"

#include "Locator.h"
#include "EnumTranslator.h"

#include "PluginDummy.h"
#include <iostream>

#include "Instructions.h"


NSVR_PLUGIN_RETURN(int) nsvr_plugin_register(nsvr_plugin_api * api)
{
	

	api->init = [](nsvr_plugin** plugin) {
	
		Locator::initialize();
		Locator::provide(new EnumTranslator());
		*plugin = AS_TYPE(nsvr_plugin, new PluginDummy());


		return 1;
	};

	api->free = [](nsvr_plugin* plugin) {
		delete AS_TYPE(PluginDummy, plugin);
		return 1;
	};

	api->configure = [](nsvr_plugin* plugin, nsvr_core* core) {
		return AS_TYPE(PluginDummy, plugin)->Configure(core);
		
	};



	return 1;
}



