#include "stdafx.h"
#include "IHardwareDevice.h"
#include "PluginInstance.h"
#include "PluginApis.h"
#include "nsvr_preset.h"

#include <boost/log/trivial.hpp>
#include "Locator.h"
#include "HardwareCoordinator.h"
#include "BodyGraph.h"

Device::Device(const DeviceDescriptor& descriptor, PluginApis& capi, PluginEventSource& ev)
	: m_name(descriptor.displayName)
	, m_apis(&capi)
	, m_trackingNodes()
	, m_hapticNodes()
	, m_isBodyGraphSetup(false)
	, m_deviceId(descriptor.id)
{
	
	if (!m_apis->Supports<device_api>()) {
		//uh oh? Make it required
	}
	else {
		dynamicallyFetchNodes();
	}


	
}



void Device::createNewNode(const NodeDescriptor& node)
{
	const auto& t = Locator::Translator();

	if (NodeDescriptor::NodeType::Haptic == node.type) {
		//todo: fix
		//BOOST_LOG_TRIVIAL(info) << "[Device] Haptic node '" << node.displayName << "' on region " << t.ToRegionString(static_cast<nsvr_region>(node.region));
		m_hapticNodes.push_back(std::make_unique<HapticNode>(node, m_apis));
	}
	else if (NodeDescriptor::NodeType::Tracker == node.type) {
		//BOOST_LOG_TRIVIAL(info) << "[Device] Tracking node '" << node.displayName << "' on region " << t.ToRegionString(static_cast<nsvr_region>(node.region));
		m_trackingNodes.push_back(std::make_unique<TrackingNode>(node, m_apis));

	}
}

void Device::parseNodes(const std::vector<NodeDescriptor>& descriptor)
{
	
	BOOST_LOG_TRIVIAL(info) << "[Device] " << m_name << " describes " << descriptor.size() << " devices:";
	for (const auto& node : descriptor) {
		createNewNode(node);
	}
}


void Device::fetchNodeInfo(uint64_t device_id) {
	device_api* enumerator = m_apis->GetApi<device_api>();
	nsvr_node_info info = { 0 };
	enumerator->submit_getnodeinfo(device_id, &info);

	NodeDescriptor desc;
	desc.capabilities = info.capabilities;
	desc.displayName = info.name;
	desc.id = device_id;
	desc.type = static_cast<NodeDescriptor::NodeType>(info.type);

	createNewNode(desc);

}

void Device::dynamicallyFetchNodes()
{
	device_api* enumerator = m_apis->GetApi<device_api>();

	nsvr_node_ids device_ids = { 0 };
	enumerator->submit_enumeratenodes(id(), &device_ids);

	std::vector<NodeDescriptor> devices;
	devices.reserve(device_ids.node_count);

	for (std::size_t i = 0; i < device_ids.node_count; i++) {
		fetchNodeInfo(device_ids.ids[i]);
	}
}




void Device::deliverRequest(const NullSpaceIPC::HighLevelEvent& event)
{
	switch (event.events_case()) {
	case NullSpaceIPC::HighLevelEvent::kSimpleHaptic:
		handleSimpleHaptic(event.parent_id(), event.simple_haptic());
		break;
	case NullSpaceIPC::HighLevelEvent::kPlaybackEvent:
		handlePlaybackEvent(event.parent_id(), event.playback_event());
		break;
	default:
		BOOST_LOG_TRIVIAL(info) << "[Device] Unrecognized request: " << event.events_case();
		break;
	}
}



void Device::handleSimpleHaptic(RequestId requestId, const NullSpaceIPC::SimpleHaptic& simple)
{
	if (auto api = m_apis->GetApi<waveform_api>()) {
		nsvr_waveform wave{};
		wave.repetitions = static_cast<std::size_t>(simple.duration() / 0.25f);
		wave.strength = simple.strength();
		wave.waveform_id = static_cast<nsvr_default_waveform>(simple.effect());

		for (uint64_t region : simple.regions()) {
			auto nodes = m_graph.getNodesForNamedRegion(static_cast<subregion::shared_region>(region));
			for (const auto& node : nodes) {
				api->submit_activate(requestId, node, reinterpret_cast<nsvr_waveform*>(&wave));
			}
		}
	} 

	else if (auto api = m_apis->GetApi<buffered_api>()) {
		double sampleDuration = 0;
		api->submit_getsampleduration(&sampleDuration);
		uint32_t numNecessarySamples = std::max<uint32_t>(1, static_cast<uint32_t>(simple.duration() / sampleDuration));

		std::vector<double> samples(numNecessarySamples, simple.strength());

		for (uint64_t region : simple.regions()) {
			auto nodes = m_graph.getNodesForNamedRegion(static_cast<subregion::shared_region>(region));
			for (const auto& node : nodes) {
				api->submit_buffer(node, samples.data(), samples.size());
			}
		}
	}
	

	
}





void Device::handlePlaybackEvent(RequestId id, const ::NullSpaceIPC::PlaybackEvent& event)
{
		if (playback_api* api = m_apis->GetApi<playback_api>()) {
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
			BOOST_LOG_TRIVIAL(warning) << "[Device] Unknown playback event: " << event.command();
			break;
		}
	}
}

//returns nullptr on failure
const Node * Device::findNode(nsvr_node_id id) const
{
	auto it = std::find_if(m_hapticNodes.begin(), m_hapticNodes.end(), [id](const std::unique_ptr<HapticNode>& node) {return node->id() == id; });
	if (it != m_hapticNodes.end()) {
		return it->get();
	}

	auto it2 = std::find_if(m_trackingNodes.begin(), m_trackingNodes.end(), [id](const std::unique_ptr<TrackingNode>& node) { return node->id() == id; });
	if (it2 != m_trackingNodes.end()) {
		return it2->get();
	}

	return nullptr;
}

Node * Device::findNode(uint64_t id)
{
	return const_cast<Node*>(static_cast<const Device&>(*this).findNode(id));
}




HapticNode::HapticNode(const NodeDescriptor& info, PluginApis*c) 
	: Node(info, c)
{

}



NodeView::Data HapticNode::Render() const
{
	if (sampling_api* api = m_apis->GetApi<sampling_api>()) {
		nsvr_sampling_sample state = { 0 };
		api->submit_query(id(), &state);	
		return NodeView::Data { state.data_0, state.data_1, state.data_2, state.intensity };
	}

	return NodeView::Data { 0, 0, 0, 0 };
}

NodeView::NodeType HapticNode::Type() const
{
	return NodeView::NodeType::Haptic;
}



uint64_t HapticNode::Id() const
{
	return m_id;
}

Node::Node(const NodeDescriptor& description, PluginApis* apis)
	: m_id{ description.id }
	, m_name{ description.displayName }
	, m_apis(apis)
	{}




uint64_t Node::id() const
{
	return m_id;
}

std::string Node::name() const
{
	return m_name;
}


std::string Device::name() const
{
	return m_name;
}



void Device::setupHooks(HardwareCoordinator & coordinator)
{
	if (m_apis->Supports<tracking_api>()) {
		for (auto& node : m_trackingNodes) {
			coordinator.Hook_TrackingSlot(node->TrackingSignal);
		}
	}
}

void Device::teardownHooks() {
	for (auto& node : m_trackingNodes) {
		node->TrackingSignal.disconnect_all_slots();
	}
	
}

void Device::setupBodyRepresentation(HumanBodyNodes & body)
{
	bodygraph_api* b = m_apis->GetApi<bodygraph_api>();
	if (b != nullptr) {
		
		b->submit_setup(reinterpret_cast<nsvr_bodygraph*>(&m_graph));
		m_isBodyGraphSetup.store(true);
	}
}

void Device::teardownBodyRepresentation(HumanBodyNodes & body)
{
	for (auto& node : m_hapticNodes) {
		body.RemoveNode(node.get());
	}
}




std::vector<NodeView> Device::renderDevices() const
{
	if (!m_isBodyGraphSetup.load()) {
		return std::vector<NodeView>{};
	}


	auto devices = m_graph.getAllNodes();
	std::vector<NodeView> fullView;
	for (const auto& kvp : devices) {
		NodeView view;
		view.region = static_cast<uint64_t>(kvp.first);
		
		for (const auto& id : kvp.second) {
			NodeView::SingleNode singleNode;
			
			const Node* possibleNode = findNode(id);
			if (possibleNode) {
				const Renderable* possibleRenderable = dynamic_cast<const Renderable*>(possibleNode);
				if (possibleRenderable) {
					singleNode.DisplayData = possibleRenderable->Render();
					singleNode.Id = id;
					singleNode.Type = possibleRenderable->Type();
					view.nodes.push_back(singleNode);
				}
			}

		}

		fullView.push_back(view);

	}
	
	return fullView;
}



uint32_t Device::id() const
{
	return m_deviceId;
}

TrackingNode::TrackingNode(const NodeDescriptor & info, PluginApis* capi)
	: Node(info, capi)
{

	BeginTracking();

}




void TrackingNode::BeginTracking()
{
	tracking_api* api = m_apis->GetApi<tracking_api>();
	api->submit_beginstreaming(reinterpret_cast<nsvr_tracking_stream*>(this), id());


	
}

void TrackingNode::EndTracking()
{
	tracking_api* api = m_apis->GetApi<tracking_api>();
	api->submit_endstreaming(id());
}

void TrackingNode::DeliverTracking(nsvr_quaternion * quat)
{
	m_latestQuat = *quat;
	TrackingSignal(id(), &m_latestQuat);
}
