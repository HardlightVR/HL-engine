#include "stdafx.h"
#include "LowLevelInterface.h"
#include "HardwareCoordinator.h"
#include "cevent_internal.h"
#include "nsvr_preset.h"
LowLevelInterface::LowLevelInterface(HardwareCoordinator & coordinator) : coordinator(coordinator)
{
}

void LowLevelInterface::RegisterDirect(nsvr_direct_handler handler, void * user_data)
{
	direct_handler = { handler, user_data };
}

void LowLevelInterface::RegisterBuffered(nsvr_buffered_handler handler, void * user_data)
{
	buffered_handler = { handler, user_data };
}

void LowLevelInterface::RegisterPreset(nsvr_preset_handler handler, void * user_data)
{
	preset_handler = { handler, user_data };
}

void LowLevelInterface::Direct(nsvr_direct_request * request)
{
	if (direct_handler.initialized()) {
		direct_handler.invoke(request);
	}
}

void LowLevelInterface::Preset(nsvr_preset_request * request)
{
	if (preset_handler.initialized()) {
		preset_handler.invoke(request);
	}
}
