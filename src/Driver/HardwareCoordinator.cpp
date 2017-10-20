#include "stdafx.h"
#include "HardwareCoordinator.h"
#include "EventDispatcher.h"
#include "DeviceContainer.h"
#include "Device.h"
#include "DriverMessenger.h"
#include "SharedTypes.h"
#include "logger.h"
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
		
		BOOST_LOG_SEV(clogger::get(), nsvr_severity_info) << "Device " << strName << " added";

		device->ForEachNode([this, device, deviceId = info.Id](Node* node) {
			
			NullSpace::SharedMemory::NodeInfo info = { 0 };
			info.Id = m_idService.FromLocal(device->parentPlugin(), device->id(), node->id()).value;
			info.DeviceId = deviceId;
			std::string nodeName = node->name();
			std::copy(nodeName.begin(), nodeName.end(), info.NodeName);
			info.Type = node->type();
			m_messenger.WriteNode(info);
		});

		device->OnReceiveTrackingUpdate([this](nsvr_region region, nsvr_quaternion* quat) {
			NullSpace::SharedMemory::Quaternion q{ quat->x, quat->y, quat->z, quat->w };
			m_messenger.WriteTracking(region, q);
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

	std::unordered_map<nsvr_region, std::vector<std::pair<NodeId<global>, RenderedNode>>> groupedByRegion;

	m_devices.EachDevice([&](Device* device) {
		device->update_visualizer(.008);
		auto nodes = device->render_visualizer();

		for (auto& node : nodes) {

			auto globalId = m_idService.FromLocal(device->parentPlugin(), device->id(), node.second.Id);

			groupedByRegion[node.first].push_back(std::make_pair(globalId, node.second));
		}

	});

	for (const auto& kvp : groupedByRegion) {
		for (const auto& idNodePair : kvp.second) {
			auto id = idNodePair.first;
			auto node = idNodePair.second;

			NullSpace::SharedMemory::RegionPair regionPair{ kvp.first, static_cast<uint32_t>(node.type), id.value};
			regionPair.Value = { node.data.data_0, node.data.data_1, node.data.data_2, node.data.data_3 };
			m_messenger.WriteBodyView(regionPair);

		}
	}
}


void HardwareCoordinator::dispatchToNodes(uint64_t parent_id, const NullSpaceIPC::LocationalEvent& event)
{
	std::unordered_map<LocalDevice, std::vector<NodeId<local>>> to_be_dispatched;

	for (const auto& globalNode : event.location().nodes().nodes()) {
		if (auto lookup = m_idService.FromGlobalNode(NodeId<global>{globalNode})) {
			to_be_dispatched[LocalDevice{ lookup->device_id, lookup->plugin }].push_back(lookup->id);
		}
	}

	for (const auto& localDevice : to_be_dispatched) {
		m_devices.Get(localDevice.first)->DispatchEvent(parent_id, event.simple_haptic(), localDevice.second);
	}
	
}

void HardwareCoordinator::dispatchToRegions(uint64_t parent_id, const NullSpaceIPC::LocationalEvent& event)
{
	std::vector<nsvr_region> casted_regions;


	casted_regions.reserve(event.location().regions().regions().size());

	for (const auto& region : event.location().regions().regions()) {
		casted_regions.push_back(region);
	}

	m_devices.EachDevice([&](Device* device) {
		device->DispatchEvent(parent_id, event.simple_haptic(), casted_regions);
	});

}

void HardwareCoordinator::SetupSubscriptions(EventDispatcher& sdkEvents)
{

	
	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kPlaybackEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.EachDevice([&](Device* device) { device->DispatchEvent(event.playback_event()); });
	});

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kLocationalEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		const auto& loc = event.locational_event().location();
		switch (loc.where_case()) {
		case NullSpaceIPC::Location::kNodes:
			dispatchToNodes(event.parent_id(), event.locational_event());
			break;
		case NullSpaceIPC::Location::kRegions:
			dispatchToRegions(event.parent_id(), event.locational_event());
			break;
		default:
			BOOST_LOG_SEV(clogger::get(), nsvr_severity_warning) << "Unknown location for event: " << loc.where_case();
			break;
		}
	});
	
}



