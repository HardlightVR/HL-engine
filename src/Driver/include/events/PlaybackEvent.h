#pragma once

#include "../PluginAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NSVR_PlaybackEvent_t NSVR_PlaybackEvent;

enum NSVR_PlaybackEvent_Command {
	NSVR_PlaybackEvent_Command_Unknown = 0,
	NSVR_PlaybackEvent_Command_Pause = 1, 
	NSVR_PlaybackEvent_Command_Unpause = 2,
	NSVR_PlaybackEvent_Command_Cancel = 3
};

NSVR_CORE_RETURN(int)NSVR_PlaybackEvent_GetCommand(const NSVR_PlaybackEvent* event, NSVR_PlaybackEvent_Command* outCommand);
NSVR_CORE_RETURN(int)NSVR_PlaybackEvent_GetId(const NSVR_PlaybackEvent* event, uint64_t* outId);



#ifdef __cplusplus
}
#endif


