#pragma once

#include "../PluginAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NSVR_AudioConfig_t NSVR_AudioConfig;


NSVR_CORE_RETURN(int)NSVR_AudioConfig_GetAudioMax(const NSVR_AudioConfig* event, int* outAudioMax);
NSVR_CORE_RETURN(int)NSVR_AudioConfig_GetAudioMin(const NSVR_AudioConfig* event, int* outAudioMin);
NSVR_CORE_RETURN(int)NSVR_AudioConfig_GetPeakTime(const NSVR_AudioConfig* event, int* outPeakTime);
NSVR_CORE_RETURN(int)NSVR_AudioConfig_GetFilter(const NSVR_AudioConfig* event, int* outFilter);



#ifdef __cplusplus
}
#endif


