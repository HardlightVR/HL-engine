#pragma once
#include "../PluginAPI.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct NSVR_BriefHapticPrimitive_t NSVR_BriefHapticPrimitive;

NSVR_CORE_RETURN(int) NSVR_BriefHapticPrimitive_GetEffect(NSVR_BriefHapticPrimitive* iota, uint32_t* outEffect);
NSVR_CORE_RETURN(int) NSVR_BriefHapticPrimitive_GetStrength(NSVR_BriefHapticPrimitive* iota, float* outStrength);
NSVR_PLUGIN_RETURN(int) NSVR_TEST(NSVR_Provider* plugin, NSVR_BriefHapticPrimitive* w);
NSVR_PLUGIN_RETURN(int) NSVR_Provider_Consume_BriefHapticPrimitive(NSVR_Provider* plugin, const NSVR_BriefHapticPrimitive* iota);

#ifdef __cplusplus
}
#endif