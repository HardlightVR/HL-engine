#include "stdafx.h"
#include "../include/events/LastingHapticPrimitive.h"
#include "LastingHapticPrimitive.h"

NSVR_CORE_RETURN(int) NSVR_LastingHapticPrimitive_GetEffect(NSVR_LastingHapticPrimitive* iota, uint32_t* outEffect) {
	*outEffect = reinterpret_cast<nsvr::events::LastingHapticPrimitive*>(iota)->Effect;
	return true;
}

NSVR_CORE_RETURN(int) NSVR_LastingHapticPrimitive_GetStrength(NSVR_LastingHapticPrimitive * iota, float * outStrength)
{
	*outStrength = reinterpret_cast<nsvr::events::LastingHapticPrimitive*>(iota)->Strength;
	return true;
}


NSVR_CORE_RETURN(int) NSVR_LastingHapticPrimitive_GetDuration(NSVR_LastingHapticPrimitive * iota, float * outDuration)
{
	*outDuration = reinterpret_cast<nsvr::events::LastingHapticPrimitive*>(iota)->Duration;
	return true;
}


