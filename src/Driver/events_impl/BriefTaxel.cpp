#include "stdafx.h"
#include "../include/events/BriefTaxel.h"
#include "BriefTaxel.h"

NSVR_CORE_RETURN(int) NSVR_BriefTaxel_GetEffect(const NSVR_BriefTaxel* iota, uint32_t* outEffect) {
	*outEffect = reinterpret_cast<const nsvr::events::BriefTaxel*>(iota)->Effect;
	return true;
}

NSVR_CORE_RETURN(int) NSVR_BriefTaxel_GetStrength(const NSVR_BriefTaxel * iota, float * outStrength)
{
	*outStrength = reinterpret_cast<const nsvr::events::BriefTaxel*>(iota)->Strength;
	return true;
}
