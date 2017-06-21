#pragma once

#include "../PluginAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NSVR_LastingHapticPrimitive_t NSVR_LastingHapticPrimitive;


NSVR_CORE_RETURN(int)NSVR_LastingHapticPrimitive_GetEffect(NSVR_LastingHapticPrimitive* interval, uint32_t* outEffect);
NSVR_CORE_RETURN(int) NSVR_LastingHapticPrimitive_GetStrength(NSVR_LastingHapticPrimitive* interval, float* outStrength);
NSVR_CORE_RETURN(int) NSVR_LastingHapticPrimitive_GetDuration(NSVR_LastingHapticPrimitive* interval, float* outDuration);

NSVR_PLUGIN_RETURN(int) NSVR_Provider_Consume_LastingHapticPrimitive(NSVR_Provider* plugin, const NSVR_LastingHapticPrimitive* interval);

#ifdef __cplusplus
}
#endif