#include "stdafx.h"
#include "cevent_internal.h"

namespace nsvr {
	namespace cevents {






		LastingHaptic::LastingHaptic(uint32_t effect, float strength, float duration, nsvr_region region) :
			request_base{}
			, effect(effect)
			, strength(strength)
			, duration(duration)
			, region(region) {}

		nsvr_request_type LastingHaptic::type() const
		{
			return request_type;
		}

	

		const nsvr_request_type LastingHaptic::request_type = nsvr_request_type_lasting_haptic;

		request_base::request_base(): handle{0}, device_id{0}
		{
		}

		
	}
}