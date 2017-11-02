#pragma once

#include <memory>
#include "PluginAPI.h"

#include "DeviceManager.h"
class PluginDummy {
public:
	int Configure(nsvr_core* core);
private:
	std::unique_ptr<DeviceManager> m_plugin;
};