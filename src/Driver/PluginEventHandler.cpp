#include "stdafx.h"
#include "PluginEventHandler.h"

PluginEventHandler::PluginEventHandler(boost::asio::io_service & io)
	: m_io(io)
{
	
}

void PluginEventHandler::Raise(nsvr_device_event_type type, uint64_t id)
{
	//this may not be thread safe.
	//should check behavior of asio 
	m_io.post([this, type, id]() {
		for (auto& handler : m_subscribers[type]) {
			handler(id);
		}
	});
}


void PluginEventHandler::Subscribe(nsvr_device_event_type type, EventHandler fn)
{
	m_subscribers[type].push_back(std::move(fn));
}
