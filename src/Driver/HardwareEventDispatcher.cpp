#include "stdafx.h"
#include "HardwareEventDispatcher.h"
#include "DeviceContainer.h"

#include "PluginInstance.h"
HardwareEventDispatcher::HardwareEventDispatcher(boost::asio::io_service & io)
	: m_io(io)
{
}

///
/// Raise a plugin event. Called within the main entry points into the core API (CoreFacade).
///
void HardwareEventDispatcher::Raise(nsvr_device_event_type type, nsvr_device_id id, PluginInstance& instance)
{
	// The goal here is to raise the event in the io thread, and not in the plugin's thread.
	// This means event delivery is asynchronous. It also means we should research the thread safety 
	// of io_service::post
	m_io.post([this, type, id, &instance]() {

		switch (type) {
		case nsvr_device_event_device_connected:
			m_onDeviceCreated(id, instance.apis(), instance.descriptor());
			break;
		case nsvr_device_event_device_disconnected:
			m_onDeviceDisconnected(id);
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "[HardwareEventDispatcher] No event handlers for event type " << type;
			break;
		}
	});
}


void HardwareEventDispatcher::OnDeviceConnected(DeviceConnected::slot_type slot)
{
	m_onDeviceCreated.connect(slot);
}

void HardwareEventDispatcher::OnDeviceDisconnected(DeviceDisconnected::slot_type slot)
{
	m_onDeviceDisconnected.connect(slot);
}
