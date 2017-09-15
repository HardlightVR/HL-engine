#include "stdafx.h"
#include "PluginAPI.h"

#include "Locator.h"
#include "EnumTranslator.h"

#include "HardlightPlugin.h"
#include <iostream>


NSVR_PLUGIN_RETURN(int) nsvr_plugin_register(nsvr_plugin_api * api)
{
	api->init = [](nsvr_plugin** plugin) {
	
		Locator::initialize();
		Locator::provide(new EnumTranslator());
		*plugin = AS_TYPE(nsvr_plugin, new HardlightPlugin());


		return 1;
	};

	api->free = [](nsvr_plugin* plugin) {
		delete AS_TYPE(HardlightPlugin, plugin);
		return 1;
	};

	api->configure = [](nsvr_plugin* plugin, nsvr_core* core) {
		nsvr_log(core, nsvr_loglevel_info, "PluginRegistration", "Hello from the hardlight plugin!");
		return AS_TYPE(HardlightPlugin, plugin)->Configure(core);
		
	};



	return 1;
}



