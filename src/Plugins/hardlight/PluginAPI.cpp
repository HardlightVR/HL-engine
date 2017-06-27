#include "stdafx.h"
#include "PluginAPI.h"
#include "events/brieftaxel.h"
#include "events/LastingTaxel.h"
#include "Locator.h"
#include "EnumTranslator.h"

#include "HardlightPlugin.h"
#include <iostream>



NSVR_PLUGIN_RETURN(int) NSVR_Provider_Consume_LastingTaxel(NSVR_Provider* plugin, const char* region, const NSVR_LastingTaxel* haptic) {
	uint64_t id;
	uint32_t effect;
	float strength;
	float duration;

	NSVR_LastingTaxel_GetId(haptic, &id);
	NSVR_LastingTaxel_GetEffect(haptic, &effect);
	NSVR_LastingTaxel_GetStrength(haptic, &strength);
	NSVR_LastingTaxel_GetDuration(haptic, &duration);

	Location location = Locator::Translator().ToLocationFromRegion(region);

	AS_TYPE(Hardlight_Mk3_ZoneDriver, plugin)->createCont(id, location, effect, strength, duration);
	
	return true;
}
NSVR_PLUGIN_RETURN(int) NSVR_Provider_Consume_BriefTaxel(NSVR_Provider* plugin, const char* region, const NSVR_BriefTaxel* iota) {
	
	uint32_t effect;
	float strength;
	Location location = Locator::Translator().ToLocationFromRegion(region);

	NSVR_BriefTaxel_GetEffect(iota, &effect);
	NSVR_BriefTaxel_GetStrength(iota, &strength);
	AS_TYPE(Hardlight_Mk3_ZoneDriver, plugin)->createOneshot(location, effect, strength);


	return true;
}



NSVR_PLUGIN_RETURN(int) NSVR_Init(NSVR_Plugin ** pluginPtr)
{
	Locator::initialize();
	Locator::provide(new EnumTranslator());

	*pluginPtr = AS_TYPE(NSVR_Plugin, new HardlightPlugin());

	return 1;
}

NSVR_PLUGIN_RETURN(int) NSVR_Configure(NSVR_Plugin* pluginPtr, NSVR_Core* core) {

	return AS_TYPE(HardlightPlugin, pluginPtr)->Configure(core);
	
}



NSVR_PLUGIN_RETURN(int) NSVR_Free(NSVR_Plugin ** ptr)
{
	delete AS_TYPE(HardlightPlugin, *ptr);
	*ptr = nullptr;
	return 1;
}





