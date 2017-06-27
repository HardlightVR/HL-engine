#include "stdafx.h"
#include "../include/events/RealtimeEvent.h"
#include "RealtimeEvent.h"

NSVR_CORE_RETURN(int) NSVR_RealtimeEvent_GetStrength(const NSVR_RealtimeEvent* event, float* outStrength) {
	*outStrength = AS_TYPE(const nsvr::events::RealtimeEvent, event)->Strength;
	return true;
}
