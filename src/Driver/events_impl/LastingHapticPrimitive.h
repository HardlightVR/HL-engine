#pragma once

typedef struct NSVR_Provider_t NSVR_Provider;

namespace nsvr {
	namespace events {
		struct LastingHapticPrimitive {
			uint32_t Effect;
			float Strength;
			float Duration;
		};

		
	}
}