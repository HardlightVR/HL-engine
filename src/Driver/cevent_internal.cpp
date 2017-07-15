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

brief_haptic_versions BriefHaptic::getVersion(unsigned int version) const
{
	if (version == 1) {
		return nsvr_cevent_brief_haptic_v1{ effect, strength, region };
	}
	
	return boost::blank{};
}




LastingHaptic::LastingHaptic(uint32_t effect, float strength, float duration, const char * region, uint64_t parent_id)
	: effect(effect)
	, strength(strength)
	, duration(duration)
	, region(region)
	, parent_id(parent_id)
{
}

nsvr::cevents::lasting_haptic_versions LastingHaptic::getVersion(unsigned int version) const
{
	if (version == 1) {
		return nsvr_cevent_lasting_haptic_v1 { parent_id, effect, strength, duration, region };
	}

	return boost::blank{};
}

PlaybackStateChange::PlaybackStateChange(uint64_t parent_id, nsvr_playback_statechange_command command)
	: parent_id(parent_id)
	, command(command)
{
}

nsvr::cevents::playback_statechange_versions PlaybackStateChange::getVersion(unsigned int version) const
{
	if (version == 1) {
		return nsvr_cevent_playback_statechange_v1{ parent_id, command };
	}

	return boost::blank{};
}

}
}