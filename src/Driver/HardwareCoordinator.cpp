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
HardwareCoordinator::HardwareCoordinator(boost::asio::io_service& io, DriverMessenger& messenger, DeviceContainer& devices )
	: m_devices(devices)
	, m_messenger(messenger)
	, m_bodyRepresentation{}
	, m_writeBodyRepresentation(io, boost::posix_time::milliseconds(16))
{
	m_devices.OnDeviceAdded([this, &body = m_bodyRepresentation](NodalDevice* device) {
		device->setupHooks(*this);
		device->setupBodyRepresentation(body);
	});

	m_devices.OnDeviceRemoved([this, &body = m_bodyRepresentation](NodalDevice* device) {
		device->teardownHooks();
		device->teardownBodyRepresentation(body);
	});

	m_writeBodyRepresentation.SetEvent([this]() { this->writeBodyRepresentation(); });
}



void HardwareCoordinator::Hook_TrackingSlot(boost::signals2::signal<void(const char*, nsvr_quaternion*)> & hook)
{
	hook.connect([this](const char* r, nsvr_quaternion* q) { hook_writeTracking(r, q); });
}

void HardwareCoordinator::hook_writeTracking(const char * region, nsvr_quaternion * quat)
{
	m_messenger.WriteTracking(region, NullSpace::SharedMemory::Quaternion{ quat->x, quat->y, quat->z, quat->w });
}

void HardwareCoordinator::writeBodyRepresentation()
{
	auto nodeView = m_bodyRepresentation.GetNodeView();
	std::cout << "Writing representation\n";
}


void HardwareCoordinator::SetupSubscriptions(EventDispatcher& sdkEvents)
{
	// For now, I'm simply forwarding the relevant events to all the devices
	// More complex behavior later

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kSimpleHaptic, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.Each([&](NodalDevice* device) {
			device->deliverRequest(event);
		});
	});

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kPlaybackEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.Each([&](NodalDevice* device) {
			device->deliverRequest(event);
		});

	});

	

	
	
}

// Must be called for a clean shutdown.
// Else, say a plugin has hooked into the tracking system and is publishing tracking updates.
// The system is shutting down and has destroyed its shared memory bridge - when suddenly the callback fires and tries to write
// or find something in shared memory. We don't want plugins doing anything as we are shutting down.

void HardwareCoordinator::Cleanup()
{
	m_devices.Each([this](NodalDevice* device) {
		device->teardownHooks();
	});
}



