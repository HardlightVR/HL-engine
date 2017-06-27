#pragma once

#include "../PluginAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NSVR_LastingTaxel_t NSVR_LastingTaxel;

NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetId(const NSVR_LastingTaxel* interval, uint64_t* outId);
NSVR_CORE_RETURN(int)NSVR_LastingTaxel_GetEffect(const NSVR_LastingTaxel* interval, uint32_t* outEffect);
NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetStrength(const NSVR_LastingTaxel* interval, float* outStrength);
NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetDuration(const NSVR_LastingTaxel* interval, float* outDuration);


#ifdef __cplusplus
}
#endif


