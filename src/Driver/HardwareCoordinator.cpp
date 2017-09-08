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
	, m_writeBodyRepresentation(io, boost::posix_time::milliseconds(8))
{
	m_devices.OnDeviceAdded([this](Device* device) {
		device->registerTrackedObjects([this](nsvr_node_id id, nsvr_quaternion* q) {
			writeTracking(id, q);
		});

		NullSpace::SharedMemory::DeviceInfo info = {0};
		info.Id = device->id();

		std::string strName = device->name();
		std::copy(strName.begin(), strName.end(), info.DeviceName);
		info.Status = Connected;
		m_messenger.WriteDevice(info);

	});

	m_devices.OnPreDeviceRemoved([this](Device* device) {
		m_messenger.UpdateDeviceStatus(device->id(), DeviceStatus::Disconnected);
	});

	m_writeBodyRepresentation.SetEvent([this]() { this->writeBodyRepresentation(); });
	m_writeBodyRepresentation.Start();

	
}



void HardwareCoordinator::writeTracking(nsvr_node_id node_id, nsvr_quaternion * quat)
{
	//todo: we need to actually take the quaternion arriving from the device and translate it to a region, based on the BodyGraph
	//todo: we need synchronization
	m_messenger.WriteTracking(node_id, NullSpace::SharedMemory::Quaternion{ quat->x, quat->y, quat->z, quat->w });
}

void HardwareCoordinator::writeBodyRepresentation()
{
	m_devices.Each([&messenger = m_messenger](Device* device) {
		device->simulate(.008);

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

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kRealtimeHaptic, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.Each([&](Device* device) {
			device->deliverRequest(event);
		});
	});

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kCurveHaptic, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.Each([&](Device* device) {
			device->deliverRequest(event);
		});
	});

	
	
}

void HardwareCoordinator::Cleanup()
{
	
}



