#pragma once
#include "PluginAPI.h"
#include <functional>
class HardlightPlugin {
public:
	HardlightPlugin();
	~HardlightPlugin();

	int handleDirectControl(NSVR_Region region, const char* data, unsigned int length);
	int registerRegions(NSVR_Region * requestedRegions);

private:

};