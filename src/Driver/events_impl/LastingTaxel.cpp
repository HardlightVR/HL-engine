#include "stdafx.h"
#include "../include/events/LastingTaxel.h"
#include "LastingTaxel.h"

NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetEffect(const NSVR_LastingTaxel* iota, uint32_t* outEffect) {
	*outEffect = reinterpret_cast<const nsvr::events::LastingTaxel*>(iota)->Effect;
	return true;
}

NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetStrength(const NSVR_LastingTaxel * iota, float * outStrength)
{
	*outStrength = reinterpret_cast<const nsvr::events::LastingTaxel*>(iota)->Strength;
	return true;
}

NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetDuration(const NSVR_LastingTaxel * iota, float * outDuration)
{
	*outDuration = reinterpret_cast<const nsvr::events::LastingTaxel*>(iota)->Duration;
	return true;
}


