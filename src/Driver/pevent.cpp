#include "stdafx.h"
#include "pevent.h"

namespace nsvr {
	namespace pevents {
		device_event::device_event(nsvr_device_event_type type) : type(type), device_id{ 0 } {}

		nsvr::pevents::device_event* device_event::make(nsvr_device_event_type type)
		{

			switch (type) {
			case nsvr_device_event_device_connected:
				return new device_connected;
			case nsvr_device_event_device_disconnected:
				return new device_disconnected;
			case nsvr_device_event_tracking_update:
				return new tracking_update;
			default:
				return nullptr;
				break;
			}

		}



		device_connected::device_connected()
			: device_event(nsvr_device_event_type::nsvr_device_event_device_connected)
		{}


		tracking_update::tracking_update()
			: device_event(nsvr_device_event_type::nsvr_device_event_tracking_update)
			, quat{}
		{
		}


		device_disconnected::device_disconnected()
			: device_event(nsvr_device_event_type::nsvr_device_event_device_disconnected)
		{

		}

	}
}