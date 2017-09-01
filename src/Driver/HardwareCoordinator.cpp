#include "stdafx.h"
#include "HardwareCoordinator.h"
#include <iostream>
#include "EventDispatcher.h"
#include "DeviceContainer.h"
#include "IHardwareDevice.h"
#include "PluginAPI.h"
#include "DriverMessenger.h"
#include "SharedTypes.h"
#include <boost/variant.hpp>
HardwareCoordinator::HardwareCoordinator(boost::asio::io_service& io, DriverMessenger& messenger, DeviceContainer& devices )
	: m_devices(devices)
	, m_messenger(messenger)
	, m_bodyRepresentation{}
	, m_writeBodyRepresentation(io, boost::posix_time::milliseconds(8))
{
	m_devices.OnDeviceAdded([this](Device* device) {
		device->setupHooks(*this);
		device->setupBodyRepresentation();
		

		NullSpace::SharedMemory::DeviceInfo info = {};
		info.Id = device->id();

		memcpy_s(info.DeviceName, 128, device->name().data(), device->name().size());
		m_messenger.WriteDevice(info);

	});

	m_devices.OnPreDeviceRemoved([this, &body = m_bodyRepresentation](Device* device) {
		device->teardownHooks();
		device->teardownBodyRepresentation(body);

		m_messenger.RemoveDevice(device->id());
	});

	m_writeBodyRepresentation.SetEvent([this]() { this->writeBodyRepresentation(); });
//	m_writeBodyRepresentation.Start();

	
}



void HardwareCoordinator::Hook_TrackingSlot(boost::signals2::signal<void(uint64_t, nsvr_quaternion*)> & hook)
{
	hook.connect([this](uint64_t r, nsvr_quaternion* q) { hook_writeTracking(r, q); });
}


void HardwareCoordinator::hook_writeTracking(nsvr_node_id node_id, nsvr_quaternion * quat)
{
	//todo: we need to actually take the quaternion arriving from the device and translate it to a region, based on the BodyGraph
	m_messenger.WriteTracking(node_id, NullSpace::SharedMemory::Quaternion{ quat->x, quat->y, quat->z, quat->w });
}

void HardwareCoordinator::writeBodyRepresentation()
{
	m_devices.Each([&messenger = m_messenger](Device* device) {

		auto nodeView = device->renderDevices();

		for (const auto& node : nodeView) {
			for (const auto& single : node.nodes) {
				NullSpace::SharedMemory::RegionPair pair;
				pair.Type = static_cast<uint32_t>(single.Type);
				pair.Region = node.region;
				pair.Id = single.Id;

				pair.Value = NullSpace::SharedMemory::Data{
					single.DisplayData.data_0,
					single.DisplayData.data_1,
					single.DisplayData.data_2,
					single.DisplayData.intensity };
				messenger.WriteBodyView(std::move(pair));
			}
		}
	});
	
	
}


void HardwareCoordinator::SetupSubscriptions(EventDispatcher& sdkEvents)
{
	// For now, I'm simply forwarding the relevant events to all the devices
	// More complex behavior later

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kSimpleHaptic, [&](const NullSpaceIPC::HighLevelEvent& event) {
		BOOST_LOG_TRIVIAL(info) << "Got haptic";
		m_devices.Each([&](Device* device) {
			device->deliverRequest(event);
		});
	});

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kPlaybackEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.Each([&](Device* device) {
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
	m_devices.Each([this](Device* device) {
		device->teardownHooks();
	});
}



