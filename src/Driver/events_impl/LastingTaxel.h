#pragma once

#include "../include/events/LastingTaxel.h"

namespace nsvr {
	namespace events {
		struct LastingTaxel {
			uint64_t Id;
			uint32_t Effect;
			float Strength;
			float Duration;
		};

		
	}
}