#include "stdafx.h"
#include "HardwareCoordinator.h"
#include <iostream>
#include "cevent_internal.h"
#include "EventDispatcher.h"
#include "DeviceContainer.h"
#include "IHardwareDevice.h"
#include "PluginAPI.h"
#include "DriverMessenger.h"
#include "SharedTypes.h"
HardwareCoordinator::HardwareCoordinator(DriverMessenger& messenger, DeviceContainer& devices, EventDispatcher& dispatcher)
	: m_devices(devices)
	, m_messenger(messenger)
{
	setupSubscriptions(dispatcher);
}



void HardwareCoordinator::RegisterTrackingSource(boost::signals2::signal<void(const char*, nsvr_quaternion*)> & hook)
{
	hook.connect([&](const char* r, nsvr_quaternion* q) { writeTracking(r, q); });
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
		device->setupHooks(*this);
	});

	m_devices.OnDeviceRemoved([&](NodalDevice* device) {
		device->teardownHooks();
	});

	
}

void HardwareCoordinator::writeTracking(const char * region, nsvr_quaternion * quat)
{
	m_messenger.WriteTracking(region, NullSpace::SharedMemory::Quaternion{ quat->x, quat->y, quat->z, quat->w });
}

