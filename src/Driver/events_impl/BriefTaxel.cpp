#include "stdafx.h"
#include "../include/events/BriefTaxel.h"
#include "BriefTaxel.h"

NSVR_CORE_RETURN(int) NSVR_BriefTaxel_GetEffect(NSVR_BriefTaxel* iota, uint32_t* outEffect) {
	*outEffect = reinterpret_cast<nsvr::events::BriefTaxel*>(iota)->Effect;
	return true;
}

NSVR_CORE_RETURN(int) NSVR_BriefTaxel_GetStrength(NSVR_BriefTaxel * iota, float * outStrength)
{
	*outStrength = reinterpret_cast<nsvr::events::BriefTaxel*>(iota)->Strength;
	return true;
}
