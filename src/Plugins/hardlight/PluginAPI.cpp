#include "stdafx.h"
#include "PluginAPI.h"
#include "events/briefhapticprimitive.h"
#include "events/LastingHapticPrimitive.h"

#include "HardlightPlugin.h"
#include <iostream>


NSVR_PLUGIN_RETURN(int) NSVR_Provider_Consume_LastingHapticPrimitive(NSVR_Provider* plugin, const NSVR_LastingHapticPrimitive* haptic) {
	float duration;
	NSVR_LastingHapticPrimitive_GetDuration(haptic, &duration);
	std::cout << "Got a lasting haptic primitive of duration " << duration << '\n';
	return true;
}
NSVR_PLUGIN_RETURN(int) NSVR_Provider_Consume_BriefHapticPrimitive(NSVR_Provider* plugin, const NSVR_BriefHapticPrimitive* iota) {
	return true;
}

NSVR_PLUGIN_RETURN(int) NSVR_Init(NSVR_Provider ** pluginPtr)
{
	*pluginPtr = AS_TYPE(NSVR_Provider, new HardlightPlugin());

	return 1;
}

NSVR_PLUGIN_RETURN(int) NSVR_Free(NSVR_Provider ** ptr)
{
	delete AS_TYPE(HardlightPlugin, *ptr);
	*ptr = nullptr;
	return 1;
}

NSVR_PLUGIN_RETURN(int) NSVR_RegisterRegions(NSVR_Provider * plugin, NSVR_Region * requestedRegions)
{
	return AS_TYPE(HardlightPlugin, plugin)->registerRegions(requestedRegions);
}

