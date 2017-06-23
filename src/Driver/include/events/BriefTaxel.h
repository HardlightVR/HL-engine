#pragma once
#include "../PluginAPI.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct NSVR_BriefTaxel_t NSVR_BriefTaxel;

NSVR_CORE_RETURN(int) NSVR_BriefTaxel_GetEffect(NSVR_BriefTaxel* iota, uint32_t* outEffect);
NSVR_CORE_RETURN(int) NSVR_BriefTaxel_GetStrength(NSVR_BriefTaxel* iota, float* outStrength);
NSVR_PLUGIN_RETURN(int) NSVR_Provider_Consume_BriefTaxel(NSVR_Provider* plugin, const NSVR_BriefTaxel* iota);

#ifdef __cplusplus
}
#endif

REGISTER_INTERFACE(BriefTaxel)