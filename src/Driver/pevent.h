#pragma once
#include "PluginAPI.h"
namespace nsvr {
	namespace pevents {



		class pevent {
		public:
			virtual ~pevent() {}
			pevent(nsvr_pevent_type type) : type(type) {}
			nsvr_pevent_type type;
			static pevent* make(nsvr_pevent_type type);
		};


		class device_connected : public pevent {
		public:
			device_connected() : pevent(nsvr_pevent_type::nsvr_pevent_device_connected) {}
		};
		nsvr::pevents::pevent* pevent::make(nsvr_pevent_type type)
		{

			switch (type) {
			case nsvr_pevent_device_connected:
				return new device_connected;
			default:
				return nullptr;
				break;
			}

		}

	}
}
