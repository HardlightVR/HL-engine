#include "stdafx.h"
#include "HardlightPlugin.h"
#include "PluginAPI.h"
#include <iostream>
HardlightPlugin::HardlightPlugin()
{

}

HardlightPlugin::~HardlightPlugin()
{
}



int HardlightPlugin::handleDirectControl(NSVR_Region region, const char * data, unsigned int length)
{
	std::cout << "Doing direct control for region " << region << '\n';
	return 1;
}

int HardlightPlugin::registerRegions(NSVR_Region * requestedRegions)
{
	*requestedRegions =NSVR_Region(0x00FF00FF);
	return 1;
}

int HardlightPlugin::Configure(NSVR_Core* core)
{
	NSVR_RegParams params = { 0 };
	params.Provider = AS_TYPE(NSVR_Provider, this);
	params.Interface = "brief-taxel";
	params.Region = "chest_left";

	NSVR_Core_RegisterNode(core, params);
	return 1;
}


