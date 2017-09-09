#pragma once
#include <string>
#include "PluginAPI.h"
struct DeviceDescriptor {
	std::string displayName;
	nsvr_device_id id;
	nsvr_device_concept concept;
};
