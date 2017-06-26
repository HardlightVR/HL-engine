#pragma once
#include "../include/events/PlaybackEvent.h"

namespace nsvr {
	namespace events {

		struct PlaybackEvent {
			NSVR_PlaybackEvent_Command Command;
			uint64_t Id;
		};

		
	}
}