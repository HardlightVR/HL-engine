#include "stdafx.h"
#include "IHardwareDevice.h"
#include "PluginInstance.h"
#include "PluginApis.h"

//std::unique_ptr<NodalDevice> device_factories::createDevice(const HardwareDescriptor& description, PluginApis& c, PluginEventHandler& e)
//{
//	if (description.concept == HardwareDescriptor::Concept::Suit) {
//		return std::unique_ptr<NodalDevice>(new SuitDevice(description,c,e ));
//	} 
//
//	return nullptr;
//}

//
//SuitDevice::SuitDevice(const HardwareDescriptor & d, PluginApis& c, PluginEventHandler& e)
//	: m_associatedPlugin(c), m_descriptor(d)
//{
//	
//}
//
//bool SuitDevice::supportsRegion(const std::string & region) const
//{
//	for (const auto& node : m_descriptor.nodes) {
//		if (region == node.region) {
//			return true;
//		}
//	}
//	return false;
//}
//
//void SuitDevice::doRequest(const nsvr::cevents::request_base& req)
//{
//
//	//we need to emulate the apis that aren't supported. So.
//	request_api* api = m_associatedPlugin.GetApi<request_api>("request");
//	if (api) {
//		api->submit_request.invoke(
//			reinterpret_cast<nsvr_request*>(
//				const_cast<nsvr::cevents::request_base*>(&req)
//			)
//		);
//	}
//	
//}
//
//void SuitDevice::controlPlayback(uint64_t id, uint32_t command)
//{
//	playback_api* api = m_associatedPlugin.GetApi<playback_api>("playback");
//	if (api) {
//		switch (command) {
//		case 1:
//			api->submit_pause.invoke(id);
//			break;
//		case 2:
//			api->submit_unpause.invoke(id);
//			break;
//		case 3:
//			api->submit_cancel.invoke(id);
//			break;
//		default:
//			BOOST_LOG_TRIVIAL(trace) << "[SuitDevice] Encountered a malformed playback request, command = " << command;
//			break;
//		}
//	}
//}






std::unique_ptr<NodalDevice> device_factories::createDevice(const HardwareDescriptor& description, PluginApis& cap, PluginEventHandler& ev)
{
	std::unique_ptr<NodalDevice> device{};
	switch (description.concept) {
	case HardwareDescriptor::Concept::Suit:
		device = std::unique_ptr<NodalDevice>(new SuitDevice{description, cap, ev});
		break;
	case HardwareDescriptor::Concept::Controller:
		//device = std::unique_ptr<NodalDevice>(new ControllerDevice{description, cap, ev});
		break;
	default:
		BOOST_LOG_TRIVIAL(info) << "[HardwareDeviceFactory] Unrecognized concept: " << (int)description.concept << " in device " << description.displayName;
		break;
	}

	return device;
}

NodalDevice::NodalDevice(std::string name, PluginApis& api)
	: m_name{name}
	, m_apis(api)
{
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
	auto regions = m_nodes.back()->getRegions();

	for (const auto& region : regions) {
		m_nodesByRegion[region].push_back(m_nodes.back().get());
	}


}

std::string NodalDevice::name() const
{
	return m_name;
}

void NodalDevice::handleSimpleHaptic(RequestId requestId, const NullSpaceIPC::SimpleHaptic& simple)
{
	if (auto api = m_apis.GetApi<request_api>("request")) {
		for (const auto& region : simple.regions()) {
			auto ev = nsvr::cevents::LastingHaptic(simple.effect(), simple.strength(), simple.duration(), region.c_str());
			ev.handle = requestId;
			api->submit_request(reinterpret_cast<nsvr_request*>(&ev));
		}
	}
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
		if (m_apis.SupportsApi("request")) {
		
			//m_apis.GetApi<request_api>("request")->submit_request()
		}
	}
}

SuitDevice::SuitDevice(HardwareDescriptor desc, PluginApis & capi, PluginEventHandler& ev)
	: NodalDevice{desc.displayName, capi}
{
	if (capi.SupportsApi("device")) {
		nsvr_device_ids ids = { 0 };
		auto api = capi.GetApi<device_api>("device");

		api->submit_enumerateids(&ids);

		for (std::size_t i = 0; i < ids.length; i++) {
			nsvr_device_basic_info info = { 0 };
			api->submit_getinfo(ids.ids[i], &info);
			
			NodeDescriptor desc;
			desc.capabilities = info.capabilities;
			desc.displayName = info.name;
			desc.id = info.id;
			desc.nodeType = (NodeDescriptor::NodeType)info.type;
			desc.region = info.region;
			if (desc.nodeType== NodeDescriptor::NodeType::Haptic) {
				this->addNode(std::unique_ptr<Node>(new HapticNode{desc, capi, ev}));
			}


		}
	}
	else {
		for (const auto& node : desc.nodes) {
	
			if (node.nodeType == NodeDescriptor::NodeType::Haptic) {
				this->addNode(std::unique_ptr<Node>(new HapticNode{ node, capi, ev }));
			}
		}
	}

}

Node::Node(uint64_t id, const std::string& name, uint32_t capability) :m_id(id), m_name{ name }, m_capability(capability) {}


uint64_t Node::id() const
{
	return m_id;
}
