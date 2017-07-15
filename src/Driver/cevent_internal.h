#pragma once
#include <stdint.h>
#include "PluginAPI.h"
//This is the place to define the most up-to-date internal representations of cevents

namespace nsvr {
namespace cevents {


//List any versions of cevents here, with their own variants as needed

//Intent: brief haptic sensation
using brief_haptic_versions = boost::variant<boost::blank
	, nsvr_cevent_brief_haptic_v1
>;


//Intent: haptic sensation with a duration
using lasting_haptic_versions = boost::variant<boost::blank
	, nsvr_cevent_lasting_haptic_v1
>;

//Intent: control the playback of a haptic effect
using playback_statechange_versions = boost::variant<boost::blank
	, nsvr_cevent_playback_statechange_v1
>;


struct BriefHaptic {
	BriefHaptic(uint32_t effect, float strength, const char* region);

	uint32_t effect;
	float strength;
	const char* region;

	const static nsvr_cevent_type event_type = nsvr_cevent_type_brief_haptic;
	nsvr::cevents::brief_haptic_versions getVersion(unsigned int version) const;
};

struct LastingHaptic {
	LastingHaptic(uint32_t effect, float strength, float duration, const char* region, uint64_t parent_id);

	uint32_t effect;
	float strength;
	float duration;
	const char* region;
	uint64_t parent_id;

	const static nsvr_cevent_type event_type = nsvr_cevent_type_lasting_haptic;
	nsvr::cevents::lasting_haptic_versions getVersion(unsigned int version) const;

};


struct PlaybackStateChange {
	PlaybackStateChange(uint64_t parent_id, nsvr_playback_statechange_command command);

	uint64_t parent_id;
	nsvr_playback_statechange_command command;

	const static nsvr_cevent_type event_type = nsvr_cevent_type_playback_statechange;
	nsvr::cevents::playback_statechange_versions getVersion(unsigned int version) const;

};

}
}