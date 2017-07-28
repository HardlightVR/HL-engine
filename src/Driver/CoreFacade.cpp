#include "stdafx.h"
#include "CoreFacade.h"
#include "PluginCapabilities.h"
#include "PluginEventHandler.h"


CoreFacade::CoreFacade(PluginCapabilities& apiRegistry, PluginEventHandler& eventHandler)
	: m_eventHandler(eventHandler)
	, m_apiRegistry(apiRegistry)
{

}



void CoreFacade::RaisePluginEvent(const nsvr::pevents::device_event& event)
{
	m_eventHandler.Raise(event);
}
