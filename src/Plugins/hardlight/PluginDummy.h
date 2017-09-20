#pragma once

#include <memory>
#include "HardlightPlugin.h"
#include "PluginAPI.h"

class PluginDummy {
public:
	PluginDummy();
	int Configure(nsvr_core* core);
private:
	std::unique_ptr<HardlightPlugin> m_plugin;
};