#pragma once

#include "PluginAPI.h"


class PluginEventSource {
public:
	virtual void Raise(nsvr_device_event_type type, nsvr_device_id id) = 0;
};

