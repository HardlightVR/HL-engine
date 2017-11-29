#include "stdafx.h"
#include "HardwareCoordinator.h"
#include "EventDispatcher.h"
#include "DeviceContainer.h"
#include "Device.h"
#include "DriverMessenger.h"
#include "SharedTypes.h"
#include "logger.h"
#include "DeviceIds.h"
#include <boost/variant/static_visitor.hpp>
#include "DriverToSharedMem.h"

class tracking_writer : public boost::static_visitor<void> {
public:
	tracking_writer(uint32_t region, DriverMessenger& messenger) : region(region), messenger(messenger) {}

	void operator()(const HardwareTracking::compass_val& compass) {
		messenger.WriteCompass(region, DriverToShmem::from(compass.value));
	}

	void operator()(const HardwareTracking::gravity_val& grav) {
		messenger.WriteGravity(region, DriverToShmem::from(grav.value));
	}

	void operator()(const HardwareTracking::quaternion_val& quat) {
		messenger.WriteQuaternion(region, DriverToShmem::from(quat));
	}
private:
	DriverMessenger& messenger;
	uint32_t region;
};
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

		device->OnReceiveTrackingUpdate([this](nsvr_region region, HardwareTracking::tracking_value val) {
			tracking_writer visitor(region, m_messenger);
			boost::apply_visitor(visitor, val);
		});

	});

	m_devices.OnDeviceRemoved([this](Device* device) {
		BOOST_LOG_SEV(clogger::get(), nsvr_severity_info) << "Device " << device->name() << " removed";


		m_messenger.UpdateDeviceStatus(m_idService.FromLocal(device->parentPlugin(), device->id()).value, DeviceStatus::Disconnected);
		device->ForEachNode([this, device](Node* node) {

			m_messenger.RemoveNode(m_idService.FromLocal(device->parentPlugin(), device->id(), node->id()).value);
		});
	});

	m_writeBodyRepresentation.SetEvent([this]() { this->writeBodyRepresentation(); });
	m_writeBodyRepresentation.Start();

	
}




void HardwareCoordinator::writeBodyRepresentation()
{

	std::unordered_map<nsvr_region, std::vector<std::pair<NodeId<global>, RenderedNode>>> groupedByRegion;

	m_devices.EachDevice([&](Device* device) {
		device->UpdateVisualizer(.008);
		auto nodes = device->RenderVisualizer();

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



 

void HardwareCoordinator::genericDispatch(uint64_t id, const NullSpaceIPC::LocationalEvent& event) {
	if (event.location().where_case() == NullSpaceIPC::Location::kNodes) {
		std::unordered_map<LocalDevice, std::vector<NodeId<local>>> toBeDispatched;

		const auto& nodes = event.location().nodes().nodes();

		for (const auto& globalNode : nodes) {
			if (auto lookup = m_idService.FromGlobalNode(NodeId<global>{globalNode})) {
				toBeDispatched[LocalDevice{ lookup->device_id, lookup->plugin }].push_back(lookup->id);
			}
		}

		for (const auto& localDevice : toBeDispatched) {
			if (auto ptr = m_devices.Get(localDevice.first)) {
				ptr->Deliver(id, event, localDevice.second);
			}
		}
	}

	else if (event.location().where_case() == NullSpaceIPC::Location::kRegions) {
		const auto& protoBufRegions = event.location().regions().regions();
		std::vector<nsvr_region> regions(protoBufRegions.begin(), protoBufRegions.end());

		m_devices.EachDevice([&](Device* device) {
			device->Deliver(id, event, regions);
		});
	}

	else {
		BOOST_LOG_SEV(clogger::get(), nsvr_severity_warning) << "Unknown 'where' case in event: " << event.location().where_case();
	}
}


void HardwareCoordinator::SetupSubscriptions(EventDispatcher& sdkEvents)
{

	
	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kPlaybackEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.EachDevice([&](Device* device) { device->Deliver(event.parent_id(), event.playback_event()); });
	});

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kLocationalEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		genericDispatch(event.parent_id(), event.locational_event());
	});

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kDeviceEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		if (auto deviceId = m_idService.FromGlobalDevice(DeviceId<global>{event.device_event().device()})) {
			if (auto device = m_devices.Get(*deviceId)) {
				device->Deliver(event.device_event());
			}
		}
	});
	
}



