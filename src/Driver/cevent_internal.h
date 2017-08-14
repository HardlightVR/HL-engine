#pragma once
#include <stdint.h>
#include "PluginAPI.h"
//This is the place to define the most up-to-date internal representations of cevents
#include "nsvr_playback_handle.h"
namespace nsvr {
namespace cevents {

	struct request_base {
		virtual ~request_base() {}
		request_base();
		virtual nsvr_request_type type() const = 0;

		virtual uint64_t getHandle() const { return handle; }
		
		uint64_t handle;
	
		uint64_t device_id;
	};

	
	


struct LastingHaptic :public request_base {
	LastingHaptic(uint32_t effect, float strength, float duration, nsvr_region region);

	uint32_t effect;
	float strength;
	float duration;
	nsvr_region region;


	static const nsvr_request_type request_type;
	nsvr_request_type type() const override;


};


}
}


