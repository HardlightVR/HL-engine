#pragma once
#include "PluginAPI.h"
namespace nsvr {
	namespace pevents {



		class device_event {
		public:
			virtual ~device_event() {}
			device_event(nsvr_device_event_type type);
			static device_event* make(nsvr_device_event_type type);
			nsvr_device_event_type type;
			uint32_t device_id;
			
		};


		class device_connected : public device_event {
		public:
			device_connected();
		};

		class tracking_update : public device_event {
		public:
			tracking_update();
			std::string region;
			NSVR_Core_Quaternion quat;
		};
		

	}
}
