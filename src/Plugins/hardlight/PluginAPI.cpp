#include "stdafx.h"
#include "PluginAPI.h"
#include "events/brieftaxel.h"
#include "events/LastingTaxel.h"
#include "Locator.h"
#include "EnumTranslator.h"

#include "HardlightPlugin.h"
#include <iostream>



NSVR_PLUGIN_RETURN(int) NSVR_Provider_Consume_LastingTaxel(NSVR_Provider* plugin, const char* region, const NSVR_LastingTaxel* haptic) {
//	float duration;
	//NSVR_LastingTaxel_GetDuration(haptic, &duration);
//	std::cout << "Got a lasting haptic primitive of duration " << duration << '\n';

	//AS_TYPE(HardlightPlugin, pluginPtr)->
	return true;
}
NSVR_PLUGIN_RETURN(int) NSVR_Provider_Consume_BriefTaxel(NSVR_Provider* plugin, const char* region, const NSVR_BriefTaxel* iota) {
	
	uint32_t effect;
	float strength;
	Location location = Locator::Translator().ToLocationFromRegion(region);

	NSVR_BriefTaxel_GetEffect(iota, &effect);
	NSVR_BriefTaxel_GetStrength(iota, &strength);

	AS_TYPE(HardlightPlugin, plugin)->PlayBrief(location, effect, strength);


	return true;
}

NSVR_PLUGIN_RETURN(int) NSVR_Init(NSVR_Provider ** pluginPtr)
{
	Locator::initialize();
	Locator::provide(new EnumTranslator());

	*pluginPtr = AS_TYPE(NSVR_Provider, new HardlightPlugin());

	return 1;
}

NSVR_PLUGIN_RETURN(int) NSVR_Configure(NSVR_Provider* pluginPtr, NSVR_Core* core) {

	return AS_TYPE(HardlightPlugin, pluginPtr)->Configure(core);
	
}



NSVR_PLUGIN_RETURN(int) NSVR_Free(NSVR_Provider ** ptr)
{
	delete AS_TYPE(HardlightPlugin, *ptr);
	*ptr = nullptr;
	return 1;
}





