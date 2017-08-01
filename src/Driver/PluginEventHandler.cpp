#include "stdafx.h"
#include "PluginEventHandler.h"

PluginEventHandler::PluginEventHandler(boost::asio::io_service & io)
	: m_io(io)
{
	
}

void PluginEventHandler::Raise(nsvr::pevents::device_event event)
{
	//this may not be thread safe.
	//should check behavior of asio 
	m_io.post([this, event]() {
		for (auto& handler : m_subscribers[event.type]) {
			handler(event);
		}
	});
}


void PluginEventHandler::Subscribe(nsvr_device_event_type type, EventHandler fn)
{
	m_subscribers[type].push_back(std::move(fn));
}
