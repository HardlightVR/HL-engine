#pragma once

#include <memory>
#include "PluginAPI.h"
#include "HardlightPlugin.h"
class HardlightPlugin;
class PluginDummy {
public:
	int Configure(nsvr_core* core);
private:
	std::unique_ptr<HardlightPlugin> m_plugin;
};