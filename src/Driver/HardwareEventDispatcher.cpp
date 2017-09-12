#include "stdafx.h"
#include "HardwareEventDispatcher.h"
#include "DeviceContainer.h"
HardwareEventDispatcher::HardwareEventDispatcher(boost::asio::io_service & io)
	: m_io(io)
	, m_deviceContainer(container)
{
}

///
/// Raise a plugin event. Called within the main entry points into the core API (CoreFacade).
///
void HardwareEventDispatcher::Raise(nsvr_device_event_type type, nsvr_device_id id)
{
	// The goal here is to raise the event in the io thread, and not in the plugin's thread.
	// This means event delivery is asynchronous. It also means we should research the thread safety 
	// of io_service::post
	m_io.post([this, type, id]() {
		auto it = m_subscribers.find(type);
		if (it != m_subscribers.end()) {
			(it->second)(id);
		}
	});
}


void HardwareEventDispatcher::Subscribe(nsvr_device_event_type type, EventSignal::slot_type fn)
{
	m_subscribers[type].connect(fn);
}
