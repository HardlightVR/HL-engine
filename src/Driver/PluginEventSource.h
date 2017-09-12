#pragma once

#include "PluginAPI.h"


class PluginInstance;
class PluginEventSource {
public:
	virtual void Raise(nsvr_device_event_type type, nsvr_device_id id, PluginInstance& instance) = 0;

};

