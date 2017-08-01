#include "stdafx.h"
#include "HardwareCoordinator.h"
#include <iostream>
#include "cevent_internal.h"
#include "EventDispatcher.h"
#include "DeviceContainer.h"

#include "PluginAPI.h"
#include "DriverMessenger.h"
#include "SharedTypes.h"
HardwareCoordinator::HardwareCoordinator(DriverMessenger& messenger, DeviceContainer& devices, EventDispatcher& dispatcher)
	: m_devices(devices)
	, m_messenger(messenger)
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

	m_devices.OnDeviceAdded([&](NodalDevice* device) {
		if (device->hasCapability(Apis::Tracking)) {
			
			device->registerTrackingHook([&](const char* region, nsvr_quaternion* quat) {
				this->writeTracking(region, quat);
			});

			device->beginTracking();
		}
	});

	
}

void HardwareCoordinator::writeTracking(const char * region, nsvr_quaternion * quat)
{
	m_messenger.WriteTracking(region, NullSpace::SharedMemory::Quaternion{ quat->x, quat->y, quat->z, quat->w });
}

