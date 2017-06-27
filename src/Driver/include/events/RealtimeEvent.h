#pragma once

#include "../PluginAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NSVR_RealtimeEvent_t NSVR_RealtimeEvent;


NSVR_CORE_RETURN(int)NSVR_RealtimeEvent_GetStrength(const NSVR_RealtimeEvent* event, float* outStrength);




#ifdef __cplusplus
}
#endif


