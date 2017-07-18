#pragma once
#include <stdint.h>
#include "PluginAPI.h"
//This is the place to define the most up-to-date internal representations of cevents

namespace nsvr {
namespace cevents {

	struct request_base {
		virtual ~request_base() {}
		virtual nsvr_request_type getType() const = 0;
	};

	
struct BriefHaptic : public request_base {
	BriefHaptic(uint32_t effect, float strength, const char* region);

	uint32_t effect;
	float strength;
	const char* region;
	static const nsvr_request_type request_type;

	nsvr_request_type getType() const override;

};

struct LastingHaptic :public request_base {
	LastingHaptic(uint32_t effect, float strength, float duration, const char* region, uint64_t parent_id);

	uint32_t effect;
	float strength;
	float duration;
	const char* region;
	uint64_t parent_id;

	static const nsvr_request_type request_type;
	nsvr_request_type getType() const override;


};


struct PlaybackStateChange : public request_base{
	PlaybackStateChange(uint64_t parent_id, nsvr_playback_statechange_command command);

	uint64_t parent_id;
	nsvr_playback_statechange_command command;
	nsvr_request_type getType() const override;

	static const nsvr_request_type request_type;

};

}
}