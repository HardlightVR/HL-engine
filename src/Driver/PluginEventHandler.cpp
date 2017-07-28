#include "stdafx.h"
#include "PluginEventHandler.h"

void PluginEventHandler::Raise(const nsvr::pevents::device_event & event)
{
	for (auto& handler : m_subscribers[event.type]) {
		handler(event);
	}
}


void PluginEventHandler::Subscribe(nsvr_device_event_type type, EventHandler fn)
{
	m_subscribers[type].push_back(std::move(fn));
}
