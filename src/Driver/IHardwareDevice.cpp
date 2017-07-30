#include "stdafx.h"
#include "IHardwareDevice.h"
#include "PluginInstance.h"
#include "PluginApis.h"
#include "nsvr_preset.h"

#include <boost/log/trivial.hpp>






std::unique_ptr<NodalDevice> device_factories::createDevice(const HardwareDescriptor& description, PluginApis& cap, PluginEventHandler& ev)
{
	std::unique_ptr<NodalDevice> device{};
	switch (description.concept) {
	case HardwareDescriptor::Concept::Suit:
		device = std::unique_ptr<NodalDevice>(new SuitDevice{description, cap, ev});
		break;
	case HardwareDescriptor::Concept::Controller:
		device = std::unique_ptr<NodalDevice>(new ControllerDevice{description, cap, ev});
		break;
	default:
		BOOST_LOG_TRIVIAL(info) << "[HardwareDeviceFactory] Unrecognized concept: " << (int)description.concept << " in device " << description.displayName;
		break;
	}

	return device;
}

NodalDevice::NodalDevice(HardwareDescriptor& descriptor, PluginApis& capi, PluginEventHandler& ev)
	: m_name{descriptor.displayName}
	, m_apis(capi)
{

	ev.Subscribe(nsvr_device_event_device_connected, [&](const nsvr::pevents::device_event& ev) {
		if (auto device = findDevice(ev.device_id)) {
			BOOST_LOG_TRIVIAL(info) << "The known device " << device->name() << " has connected";
		}
		else {
			BOOST_LOG_TRIVIAL(info) << "Previously unknown device has connected (id = " << ev.device_id << ")";
			if (auto api = m_apis.GetApi<device_api>("device")) {
				nsvr_device_basic_info info{};
				api->submit_getinfo(ev.device_id, &info);
				BOOST_LOG_TRIVIAL(info) << "	Okay, the device is called " << info.name;

			}
		}
	});

	if (capi.SupportsApi("device")) {
		nsvr_device_ids ids = { 0 };
		auto api = capi.GetApi<device_api>("device");

		api->submit_enumerateids(&ids);

		for (std::size_t i = 0; i < ids.device_count; i++) {
			nsvr_device_basic_info info = { 0 };
			api->submit_getinfo(ids.ids[i], &info);

			NodeDescriptor desc;
			desc.capabilities = info.capabilities;
			desc.displayName = info.name;
			desc.id = ids.ids[i];
			desc.nodeType = (NodeDescriptor::NodeType)info.type;
			desc.region = info.region;
			if (desc.nodeType == NodeDescriptor::NodeType::Haptic) {
				BOOST_LOG_TRIVIAL(info) << "The device system " << m_name << " has dynamically registered a device " << info.name <<" at region " << info.region;
				this->addNode(std::unique_ptr<Node>(new HapticNode{ desc, capi, ev }));
			}


		}
	}
	else {
		for (const auto& node : descriptor.nodes) {

			if (node.nodeType == NodeDescriptor::NodeType::Haptic) {
				this->addNode(std::unique_ptr<Node>(new HapticNode{ node, capi, ev }));
			}
		}
	}
}

void NodalDevice::deliverRequest(const NullSpaceIPC::HighLevelEvent& event)
{
	switch (event.events_case()) {
	case NullSpaceIPC::HighLevelEvent::kSimpleHaptic:
		handleSimpleHaptic(event.parent_id(), event.simple_haptic());
		break;
	case NullSpaceIPC::HighLevelEvent::kPlaybackEvent:
		handlePlaybackEvent(event.parent_id(), event.playback_event());
		break;
	default:
		BOOST_LOG_TRIVIAL(info) << "[NodalDevice] Unrecognized request: " << event.events_case();
		break;
	}
}

void NodalDevice::addNode(std::unique_ptr<Node> node)
{
	m_nodes.push_back(std::move(node));
	auto region = m_nodes.back()->getRegion();

	
	m_nodesByRegion[region].push_back(m_nodes.back().get());
	


}

std::string NodalDevice::name() const
{
	return m_name;
}

void NodalDevice::handleSimpleHaptic(RequestId requestId, const NullSpaceIPC::SimpleHaptic& simple)
{
	for (const auto& region : simple.regions()) {
		auto ev = nsvr::cevents::LastingHaptic(simple.effect(), simple.strength(), simple.duration(), region.c_str());
		
		if (auto api = m_apis.GetApi<request_api>("request")) {
			ev.handle = requestId;
			api->submit_request(reinterpret_cast<nsvr_request*>(&ev));
		}
		
		else if (auto api = m_apis.GetApi<preset_api>("preset")) {
			auto& interested = m_nodesByRegion[region];
			for (auto& node : interested) {
				node->deliver(requestId, ev);
			}
		}
	}
	
}



Node * NodalDevice::findDevice(uint64_t id)
{
	for (auto& node : m_nodes) {
		if (node->id() == id) {
			return node.get();
		}
	}

	return nullptr;
}

void NodalDevice::handlePlaybackEvent(uint64_t id, const ::NullSpaceIPC::PlaybackEvent& event)
{
	if (auto api = m_apis.GetApi<playback_api>("playback")) {
		switch (event.command()) {
		case NullSpaceIPC::PlaybackEvent_Command_UNPAUSE:
			api->submit_unpause(id);
			break;
		case NullSpaceIPC::PlaybackEvent_Command_PAUSE:
			api->submit_pause(id);
			break;
		case NullSpaceIPC::PlaybackEvent_Command_CANCEL:
			api->submit_cancel(id);
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "[NodalDevice] Unknown playback event: " << event.command();
			break;
		}
	}
}

HapticNode::HapticNode(const NodeDescriptor& info, PluginApis &c, PluginEventHandler &e) 
	: Node { info.id, info.displayName, info.capabilities }
	, m_apis(c)
	, m_region{info.region}

{

}

void HapticNode::deliver(RequestId, const nsvr::cevents::request_base & base)
{
	if (base.type() == nsvr_request_type_lasting_haptic) {
		const auto lasting = static_cast<const nsvr::cevents::LastingHaptic&>(base);
		if (auto api = m_apis.GetApi<buffered_api>("buffered")) {
		
			//api->submit_buffer()
		}
		else if (auto api = m_apis.GetApi<preset_api>("preset")) {
			nsvr_preset_request req{};
			req.family = static_cast<nsvr_preset_family>(lasting.effect);
			req.strength = lasting.strength;
			api->submit_preset(reinterpret_cast<nsvr_preset_request*>(&req));
		}
	}
}

std::string HapticNode::getRegion() const
{
	return m_region;
}

SuitDevice::SuitDevice(HardwareDescriptor desc, PluginApis & capi, PluginEventHandler& ev)
	: NodalDevice{desc, capi, ev}
{
	

}

Node::Node(uint64_t id, const std::string& name, uint32_t capability) :m_id(id), m_name{ name }, m_capability(capability) {}


uint64_t Node::id() const
{
	return m_id;
}

std::string Node::name() const
{
	return m_name;
}

ControllerDevice::ControllerDevice(HardwareDescriptor desc, PluginApis & cap, PluginEventHandler & ev)
	: NodalDevice{desc, cap, ev}
{
}
