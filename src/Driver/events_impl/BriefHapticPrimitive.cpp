#include "stdafx.h"
#include "../include/events/BriefHapticPrimitive.h"
#include "BriefHapticPrimitive.h"

NSVR_CORE_RETURN(int) NSVR_BriefHapticPrimitive_GetEffect(NSVR_BriefHapticPrimitive* iota, uint32_t* outEffect) {
	*outEffect = reinterpret_cast<nsvr::events::BriefHapticPrimitive*>(iota)->Effect;
	return true;
}

NSVR_CORE_RETURN(int) NSVR_BriefHapticPrimitive_GetStrength(NSVR_BriefHapticPrimitive * iota, float * outStrength)
{
	*outStrength = reinterpret_cast<nsvr::events::BriefHapticPrimitive*>(iota)->Strength;
	return true;
}
