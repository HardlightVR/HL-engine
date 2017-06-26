#include "stdafx.h"
#include "../include/events/PlaybackEvent.h"
#include "PlaybackEvent.h"

NSVR_CORE_RETURN(int) NSVR_PlaybackEvent_GetCommand(const NSVR_PlaybackEvent* event, NSVR_PlaybackEvent_Command* outCommand) {
	*outCommand = reinterpret_cast<const nsvr::events::PlaybackEvent*>(event)->Command;
	return true;
}

NSVR_CORE_RETURN(int) NSVR_PlaybackEvent_GetId(const NSVR_PlaybackEvent* event, uint64_t* outId) {
	*outId = reinterpret_cast<const nsvr::events::PlaybackEvent*>(event)->Id;
	return true;
}


