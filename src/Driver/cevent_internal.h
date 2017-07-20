#pragma once
#include <stdint.h>
#include "PluginAPI.h"
//This is the place to define the most up-to-date internal representations of cevents
#include "nsvr_playback_handle.h"
namespace nsvr {
namespace cevents {

	struct request_base {
		virtual ~request_base() {}

		virtual nsvr_request_type getType() const = 0;

		virtual nsvr_playback_handle* getHandle() const { return nullptr; }
		
		nsvr_playback_handle* handle;
	
	};

	
	


struct LastingHaptic :public request_base {
	LastingHaptic(uint32_t effect, float strength, float duration, const char* region);

	uint32_t effect;
	float strength;
	float duration;
	const char* region;


	static const nsvr_request_type request_type;
	nsvr_request_type getType() const override;


};


}
}


