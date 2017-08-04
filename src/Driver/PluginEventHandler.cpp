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
		auto it = m_subscribers.find(type);
		if (it != m_subscribers.end()) {
			(it->second)(id);
		}
	});
}


void PluginEventHandler::Subscribe(nsvr_device_event_type type, EventSignal::slot_type fn)
{
	m_subscribers[type].connect(fn);
}
