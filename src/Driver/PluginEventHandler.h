#pragma once
#include "pevent.h"

class PluginEventHandler {
public:
	void Raise(const nsvr::pevents::device_event& event);

};