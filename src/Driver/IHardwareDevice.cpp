#include "stdafx.h"
#include "IHardwareDevice.h"
#include "PluginInstance.h"
#include "PluginApis.h"
#include "nsvr_preset.h"

#include <boost/log/trivial.hpp>

#include "HardwareCoordinator.h"
#include "HumanBodyNodes.h"
NodalDevice::NodalDevice(const HardwareDescriptor& descriptor, PluginApis& capi, PluginEventHandler& ev)
	: m_concept(descriptor.concept)
	, m_name(descriptor.displayName)
	, m_apis(&capi)
	, m_trackingDevices()
	, m_hapticDevices()
{

	


	setupSubscriptions(ev);
	
	if (!m_apis->SupportsApi(Apis::Device)) {
		parseDevices(descriptor.nodes);
	}
	else {
		fetchDynamicDevices();
	}



	
}

void NodalDevice::figureOutCapabilities()
{
}

void NodalDevice::setupSubscriptions(PluginEventHandler & ev)
{
	ev.Subscribe(nsvr_device_event_device_connected, [this](const nsvr::pevents::device_event& ev) {
		if (auto event = ev.get<nsvr::pevents::connected_event>()) {
			handle_connect(*event);
		}
	});

	ev.Subscribe(nsvr_device_event_device_disconnected, [&](const nsvr::pevents::device_event& ev) {
		if (auto event = ev.get<nsvr::pevents::disconnected_event>()) {
			handle_disconnect(*event);
		}
	});
}

void NodalDevice::parseDevices(const std::vector<NodeDescriptor>& descriptor)
{
	BOOST_LOG_TRIVIAL(info) << "[NodalDevice] " << m_name << " describes " << descriptor.size() << " devices:";
	for (const auto& node : descriptor) {
		if (NodeDescriptor::NodeType::Haptic == node.nodeType) {
			BOOST_LOG_TRIVIAL(info) << "[NodalDevice] Haptic node '" << node.displayName << "' on region " << node.region;
			m_hapticDevices.push_back(std::make_unique<HapticNode>(node, m_apis));
		}
		else if (NodeDescriptor::NodeType::Tracker == node.nodeType) {
			BOOST_LOG_TRIVIAL(info) << "[NodalDevice] Tracking node '" << node.displayName << "' on region " << node.region;
			m_trackingDevices.push_back(std::make_unique<TrackingNode>(node, m_apis));

		}
	}
}




void NodalDevice::fetchDynamicDevices()
{
	device_api* enumerator = m_apis->GetApi<device_api>();

	nsvr_device_ids device_ids = { 0 };
	enumerator->submit_enumerateids(&device_ids);

	std::vector<NodeDescriptor> nodes;
	nodes.reserve(device_ids.device_count);

	for (std::size_t i = 0; i < device_ids.device_count; i++) {
		nsvr_device_basic_info info = { 0 };
		enumerator->submit_getinfo(device_ids.ids[i], &info);

		NodeDescriptor desc;
		desc.capabilities = info.capabilities;
		desc.displayName = info.name;
		desc.id = device_ids.ids[i];
		desc.nodeType = static_cast<NodeDescriptor::NodeType>(info.type);
		desc.region = info.region;

		nodes.push_back(desc);
	}

	parseDevices(nodes);
}



void NodalDevice::handle_connect(const nsvr::pevents::connected_event &e)
{
	BOOST_LOG_TRIVIAL(info) << "[NodalDevice][" << m_name << "] A device was connected!";
}

void NodalDevice::handle_disconnect(const nsvr::pevents::disconnected_event &)
{
	BOOST_LOG_TRIVIAL(info) << "[NodalDevice][" << m_name << "] A device was disconnected!";
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



void NodalDevice::handleSimpleHaptic(RequestId requestId, const NullSpaceIPC::SimpleHaptic& simple)
{
	for (const auto& region : simple.regions()) {
		auto ev = nsvr::cevents::LastingHaptic(simple.effect(), simple.strength(), simple.duration(), region.c_str());

		if (auto api = m_apis->GetApi<request_api>()) {
			ev.handle = requestId;
			api->submit_request(reinterpret_cast<nsvr_request*>(&ev));
		}

		else if (auto api = m_apis->GetApi<preset_api>()) {
			auto& interested = m_nodesByRegion[region];
			for (auto& node : interested) {
				node->deliver(requestId, ev);
			}
		}
	}
}





void NodalDevice::handlePlaybackEvent(RequestId id, const ::NullSpaceIPC::PlaybackEvent& event)
{
	if (playback_api* api = nullptr) {

//	if (playback_api* api = m_apis.GetApi<playback_api>()) {
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




HapticNode::HapticNode(const NodeDescriptor& info, PluginApis*c) 
	: Node { info.id, info.displayName, info.capabilities }
	, m_apis(c)
	, m_region{info.region}

{

}

void HapticNode::deliver(RequestId, const nsvr::cevents::request_base & base)
{
	if (base.type() == nsvr_request_type_lasting_haptic) {
		const auto lasting = static_cast<const nsvr::cevents::LastingHaptic&>(base);
		if (auto api = m_apis->GetApi<buffered_api>()) {
		
			//api->submit_buffer()
		}
		else if (auto api = m_apis->GetApi<preset_api>()) {
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

NodeView::Data HapticNode::Render() const
{
	if (sampling_api* api = m_apis->GetApi<sampling_api>()) {
		nsvr_sampling_nodestate state;
	
		api->submit_query(m_region.c_str(), &state);
		if (nsvr_sampling_nodestate::nsvr_sampling_nodestate_active == state) {
			return NodeView::Intensity{ 1, 0 };
		}
		else {
			return NodeView::Intensity{ 0, 0 };
		}

	}
	return NodeView::Intensity{ 0, 0 };
}

NodeView::NodeType HapticNode::Type() const
{
	return NodeView::NodeType::Haptic;
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



std::string NodalDevice::name() const
{
	return m_name;
}

bool NodalDevice::hasCapability(Apis name) const
{
	return m_apis->SupportsApi(name);
}

void NodalDevice::setupHooks(HardwareCoordinator & coordinator)
{
	if (m_apis->SupportsApi<tracking_api>()) {
		for (auto& node : m_trackingDevices) {
			coordinator.Hook_TrackingSlot(node->TrackingSignal);
		}
	}

	if (m_apis->SupportsApi<sampling_api>()) {
		for (auto& node : m_hapticDevices) {
		//	coordinator.Hook_SamplingSlot(node->SamplingSignal);
		}
	}
}

void NodalDevice::teardownHooks() {
	for (auto& node : m_trackingDevices) {
		node->TrackingSignal.disconnect_all_slots();
	}
	
}

void NodalDevice::setupBodyRepresentation(HumanBodyNodes & body)
{
	for (auto& node : m_hapticDevices) {
		body.AddNode(HumanBodyNodes::NamedRegion::Arm_Lower_Left, node.get());
	}
}

void NodalDevice::teardownBodyRepresentation(HumanBodyNodes & body)
{
	for (auto& node : m_hapticDevices) {
		body.RemoveNode(node.get());
	}
}




TrackingNode::TrackingNode(const NodeDescriptor & info, PluginApis* capi)
	: Node(info.id, info.displayName, info.capabilities)
	, m_region(info.region)
	, m_apis(capi)
{

	BeginTracking();

}

void TrackingNode::deliver(RequestId, const nsvr::cevents::request_base &)
{

}

std::string TrackingNode::getRegion() const
{
	return m_region;
}

void TrackingNode::BeginTracking()
{
	tracking_api* api = m_apis->GetApi<tracking_api>();
	api->submit_beginstreaming(reinterpret_cast<nsvr_tracking_stream*>(this), m_region.c_str());

}

void TrackingNode::EndTracking()
{
	tracking_api* api = m_apis->GetApi<tracking_api>();
	api->submit_endstreaming(m_region.c_str());
}

void TrackingNode::DeliverTracking(nsvr_quaternion * quat)
{
	m_latestQuat = *quat;
	TrackingSignal(m_region.c_str(), &m_latestQuat);
}
