#pragma once

#include "../PluginAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NSVR_LastingTaxel_t NSVR_LastingTaxel;

NSVR_CORE_RETURN(int)NSVR_LastingTaxel_GetEffect(const NSVR_LastingTaxel* interval, uint32_t* outEffect);
NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetStrength(const NSVR_LastingTaxel* interval, float* outStrength);
NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetDuration(const NSVR_LastingTaxel* interval, float* outDuration);

NSVR_PLUGIN_RETURN(int) NSVR_Provider_Consume_LastingTaxel(NSVR_Provider* plugin, const NSVR_LastingTaxel* interval);

#ifdef __cplusplus
}
#endif

REGISTER_INTERFACE(LastingTaxel)

