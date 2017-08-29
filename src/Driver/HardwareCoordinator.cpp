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
	, m_pluginEventLoopInterval(boost::posix_time::millisec(16))
	, m_pluginEventLoop(io, m_pluginEventLoopInterval)
{
	m_devices.OnSystemAdded([this, &body = m_bodyRepresentation](DeviceSystem* system) {
		system->setupHooks(*this);
		system->setupBodyRepresentation(body);
		

		NullSpace::SharedMemory::SystemInfo info = {};
		info.Id = system->id();

		memcpy_s(info.SystemName, 128, system->name().data(), system->name().size());
		m_messenger.WriteSystem(info);

	});

	m_devices.OnPreSystemRemoved([this, &body = m_bodyRepresentation](DeviceSystem* system) {
		system->teardownHooks();
		system->teardownBodyRepresentation(body);

		m_messenger.RemoveSystem(system->id());
	});

	m_writeBodyRepresentation.SetEvent([this]() { this->writeBodyRepresentation(); });
	m_writeBodyRepresentation.Start();

	m_pluginEventLoop.SetEvent([this]() {
		this->runPluginUpdateLoops(m_pluginEventLoopInterval.total_milliseconds());
	});
	m_pluginEventLoop.Start();
}



void HardwareCoordinator::Hook_TrackingSlot(boost::signals2::signal<void(nsvr_region, nsvr_quaternion*)> & hook)
{
	hook.connect([this](nsvr_region r, nsvr_quaternion* q) { hook_writeTracking(r, q); });
}

void HardwareCoordinator::runPluginUpdateLoops(uint64_t dt)
{
	m_devices.Each([delta_time = dt](DeviceSystem* device) {
		device->run_update_loop_once(delta_time);
	});
}

void HardwareCoordinator::hook_writeTracking(nsvr_region region, nsvr_quaternion * quat)
{
	m_messenger.WriteTracking(region, NullSpace::SharedMemory::Quaternion{ quat->x, quat->y, quat->z, quat->w });
}

void HardwareCoordinator::writeBodyRepresentation()
{
	m_devices.Each([&messenger = m_messenger](DeviceSystem* device) {

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
		m_devices.Each([&](DeviceSystem* device) {
			device->deliverRequest(event);
		});
	});

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kPlaybackEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.Each([&](DeviceSystem* device) {
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
	m_devices.Each([this](DeviceSystem* device) {
		device->teardownHooks();
	});
}



