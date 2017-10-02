#include "stdafx.h"
#include "HardwareCoordinator.h"
#include <iostream>
#include "EventDispatcher.h"
#include "DeviceContainer.h"
#include "PluginAPI.h"
#include "Device.h"
#include "DriverMessenger.h"
#include "SharedTypes.h"
#include <boost/variant.hpp>
#include "DeviceIds.h"
HardwareCoordinator::HardwareCoordinator(boost::asio::io_service& io, DriverMessenger& messenger, DeviceContainer& devices )
	: m_devices(devices)
	, m_messenger(messenger)
	, m_writeBodyRepresentation(io, boost::posix_time::milliseconds(8))
	, m_idService()
{
	m_devices.OnDeviceAdded([this](Device* device) {
	
		NullSpace::SharedMemory::DeviceInfo info = {0};
		info.Id = m_idService.FromLocal(device->parentPlugin(), device->id()).value;

		std::string strName = device->name();
		std::copy(strName.begin(), strName.end(), info.DeviceName);
		info.Status = Connected;
		info.Concept = static_cast<uint32_t>(device->concept());
		m_messenger.WriteDevice(info);
		
		device->ForEachNode([this, device, deviceId = info.Id](Node* node) {
			
			NullSpace::SharedMemory::NodeInfo info = { 0 };
			info.Id = m_idService.FromLocal(device->parentPlugin(), device->id(), node->id()).value;
			info.DeviceId = deviceId;
			std::string nodeName = node->name();
			std::copy(nodeName.begin(), nodeName.end(), info.NodeName);
			info.Type = node->type();
			m_messenger.WriteNode(info);
		});

	});

	m_devices.OnDeviceRemoved([this](Device* device) {
		m_messenger.UpdateDeviceStatus(m_idService.FromLocal(device->parentPlugin(), device->id()).value, DeviceStatus::Disconnected);
		device->ForEachNode([this, device](Node* node) {

			m_messenger.RemoveNode(m_idService.FromLocal(device->parentPlugin(), device->id(), node->id()).value);
		});
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
	m_devices.EachSimulation([&messenger = m_messenger, this](SimulatedDevice* device) {
		device->simulate(.008);

		auto nodeView = device->render();

		for (const auto& node : nodeView) {
			for (const auto& single : node.nodes) {
				NullSpace::SharedMemory::RegionPair pair;
				pair.Type = static_cast<uint32_t>(single.Type);
				pair.Region = node.region;
				//pair.Id = m_idService.FromLocal(device->parentPlugin(), device->id(),  single.Id).value;
				//todo: fix the id. Main problem is the sim tries to mirror the real device interface wise, so I have to keep them in sync..
				//either make them inherit from same interface, or have a way of getting at the real device 

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
	/*
	sdkEvents.Subscribe(
	{ 
		NullSpaceIPC::HighLevelEvent::kSimpleHaptic,
		NullSpaceIPC::HighLevelEvent::kPlaybackEvent,
		NullSpaceIPC::HighLevelEvent::kRealtimeHaptic,
		NullSpaceIPC::HighLevelEvent::kCurveHaptic
	}, 
	[&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.EachDevice([&](Device* device) {
			device->DispatchEvent(event);
		});

	
	});
*/
	
	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kPlaybackEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.EachDevice([&](Device* device) { device->DispatchEvent(event.playback_event()); });
	});

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kLocationalEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		const auto& loc = event.locational_event().location();
		if (loc.where_case() == NullSpaceIPC::Location::kNodes) {
		
			std::unordered_map<LocalDevice, std::vector<NodeId<local>>> all_nodes;
			for (const auto& node : loc.nodes().nodes()) {
				
				if (auto possibleNode = m_idService.FromGlobalNode(NodeId<global>{node})) {
					all_nodes[LocalDevice{ possibleNode->device_id, possibleNode->plugin }].push_back(NodeId<local>{possibleNode->id});
				}
			}
			
			for (const auto& kvp : all_nodes) {
				Device* d = m_devices.Get(kvp.first.plugin, DeviceId<local>{kvp.first.id});
				if (d) {
					d->DispatchEvent(event.parent_id(), event.locational_event().simple_haptic(), kvp.second);
				}
			}
		}
		else {
			std::vector<nsvr_region> regions;
			for (const auto& region : loc.regions().regions()) {
				regions.push_back(region);
			}

			m_devices.EachDevice([&](Device* device) {
				device->DispatchEvent(event.parent_id(), event.locational_event().simple_haptic(), regions);
			});
		}
	
	});
		
	
}



