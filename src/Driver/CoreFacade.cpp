#include "stdafx.h"
#include "CoreFacade.h"
#include "PluginApis.h"
#include "PluginEventHandler.h"


CoreFacade::CoreFacade(PluginApis& capabilities, PluginEventHandler& eventHandler)
	: m_eventHandler(eventHandler)
	, m_pluginCapabilities(capabilities)
{

}



void CoreFacade::RaisePluginEvent(nsvr_device_event_type type, uint64_t id)
{
	m_eventHandler.Raise(type, id);
}
