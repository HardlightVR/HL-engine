#pragma once

#include <boost/signals2.hpp>
#include "PluginAPI.h"

class PluginEventSink {
public:
	using EventSignal = boost::signals2::signal<void(nsvr_device_id)>;
	virtual void Subscribe(nsvr_device_event_type type, EventSignal::slot_type) = 0;
};