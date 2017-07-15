#include "stdafx.h"
#include "pevent.h"

namespace nsvr {
namespace pevents {

nsvr::pevents::pevent* pevent::make(nsvr_pevent_type type)
{

	switch (type) {
	case nsvr_pevent_device_connected:
		return new device_connected;
	case nsvr_pevent_tracking_update:
		return new tracking_update;
	default:
		return nullptr;
		break;
	}

}

device_connected::device_connected() : pevent(nsvr_pevent_type::nsvr_pevent_device_connected) {}


tracking_update::tracking_update() : pevent(nsvr_pevent_type::nsvr_pevent_tracking_update), quat{}
{
}

}
}