#include "stdafx.h"
#include "../include/events/LastingHapticPrimitive.h"
#include "LastingHapticPrimitive.h"

NSVR_CORE_RETURN(int) NSVR_LastingHapticPrimitive_GetEffect(const NSVR_LastingHapticPrimitive* iota, uint32_t* outEffect) {
	*outEffect = reinterpret_cast<const nsvr::events::LastingHapticPrimitive*>(iota)->Effect;
	return true;
}

NSVR_CORE_RETURN(int) NSVR_LastingHapticPrimitive_GetStrength(const NSVR_LastingHapticPrimitive * iota, float * outStrength)
{
	*outStrength = reinterpret_cast<const nsvr::events::LastingHapticPrimitive*>(iota)->Strength;
	return true;
}


NSVR_CORE_RETURN(int) NSVR_LastingHapticPrimitive_GetDuration(const NSVR_LastingHapticPrimitive * iota, float * outDuration)
{
	*outDuration = reinterpret_cast<const nsvr::events::LastingHapticPrimitive*>(iota)->Duration;
	return true;
}


