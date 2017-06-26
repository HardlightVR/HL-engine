#include "stdafx.h"
#include "../include/events/LastingTaxel.h"
#include "LastingTaxel.h"

NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetEffect(const NSVR_LastingTaxel* iota, uint32_t* outEffect) {
	*outEffect = AS_TYPE(const nsvr::events::LastingTaxel, iota)->Effect;
	return true;
	
}

NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetStrength(const NSVR_LastingTaxel * iota, float * outStrength)
{
	*outStrength = AS_TYPE(const nsvr::events::LastingTaxel, iota)->Strength;
	return true;
}

NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetDuration(const NSVR_LastingTaxel * iota, float * outDuration)
{
	*outDuration = AS_TYPE(const nsvr::events::LastingTaxel, iota)->Duration;
	return true;
}

NSVR_CORE_RETURN(int) NSVR_LastingTaxel_GetId(const NSVR_LastingTaxel * iota, uint64_t* outId)
{
	*outId = AS_TYPE(const nsvr::events::LastingTaxel, iota)->Id;
	return true;
}


