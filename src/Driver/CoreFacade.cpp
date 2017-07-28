#include "stdafx.h"
#include "CoreFacade.h"
#include "PluginApiRegistry.h"
#include "PluginEventHandler.h"


CoreFacade::CoreFacade(PluginApiRegistry& apiRegistry, PluginEventHandler& eventHandler)
	: m_eventHandler(eventHandler)
	, m_apiRegistry(apiRegistry)
{

}



void CoreFacade::RaisePluginEvent(const nsvr::pevents::device_event& event)
{
	m_eventHandler.Raise(event);
}

void CoreFacade::RegisterPluginApi(nsvr_plugin_sampling_api* api)
{
	m_apiRegistry.Register(api);
}

void CoreFacade::RegisterPluginApi(nsvr_plugin_playback_api * api)
{
	m_apiRegistry.Register(api);
}

void CoreFacade::RegisterPluginApi(nsvr_plugin_preset_api* api)
{
	m_apiRegistry.Register(api);
}

void CoreFacade::RegisterPluginApi(nsvr_plugin_buffer_api * api)
{
	m_apiRegistry.Register(api);

}

