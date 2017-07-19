#include "stdafx.h"
#include "cevent_internal.h"

namespace nsvr {
	namespace cevents {



		BriefHaptic::BriefHaptic(uint32_t effect, float strength, const char * region)
			: effect(effect)
			, strength(strength)
			, region(region)
		{
		}

		nsvr_request_type BriefHaptic::getType() const
		{
			return request_type;
		}

		const nsvr_request_type BriefHaptic::request_type = nsvr_request_type_brief_haptic;



		LastingHaptic::LastingHaptic(uint32_t effect, float strength, float duration, const char * region, uint64_t parent_id)
			: effect(effect)
			, strength(strength)
			, duration(duration)
			, region(region)
			, parent_id(parent_id)
		{
		}

		nsvr_request_type LastingHaptic::getType() const
		{
			return request_type;
		}

		const nsvr_request_type LastingHaptic::request_type = nsvr_request_type_lasting_haptic;

		/*laybackStateChange::PlaybackStateChange(uint64_t parent_id, nsvr_playback_statechange_command command)
			: parent_id(parent_id)
			, command(command)
		{
		}

		nsvr_request_type PlaybackStateChange::getType() const
		{
			return request_type;
		}

		const nsvr_request_type PlaybackStateChange::request_type = nsvr_request_type_playback_statechange;
		}
		}*/
	}
}