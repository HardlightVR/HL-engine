#pragma once
#include "PluginAPI.h"
namespace nsvr {
	namespace pevents {



		class pevent {
		public:
			virtual ~pevent() {}
			pevent(nsvr_pevent_type type) : type(type) {}
			nsvr_pevent_type type;
			uint32_t device_id;
			static pevent* make(nsvr_pevent_type type);
		};


		class device_connected : public pevent {
		public:
			device_connected();
		};

		class tracking_update : public pevent {
		public:
			tracking_update();
			NSVR_Core_Quaternion quat;
		};
		

	}
}
