#include "stdafx.h"
#include "HardwareCoordinator.h"
#include <iostream>
#include "cevent_internal.h"
#include "EventDispatcher.h"
#include "DeviceContainer.h"


HardwareCoordinator::HardwareCoordinator(DeviceContainer& devices, EventDispatcher& dispatcher)
	: m_devices(devices)
{
	setupSubscriptions(dispatcher);
}


void HardwareCoordinator::setupSubscriptions(EventDispatcher& dispatcher)
{
	// For now, I'm simply forwarding the relevant events to all the devices
	// More complex behavior later

	dispatcher.Subscribe(NullSpaceIPC::HighLevelEvent::kSimpleHaptic, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.All([&](NodalDevice* device) {
			device->deliverRequest(event);
		});
	});

	dispatcher.Subscribe(NullSpaceIPC::HighLevelEvent::kPlaybackEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.All([&](NodalDevice* device) {
			device->deliverRequest(event);
		});
	});

	
}

