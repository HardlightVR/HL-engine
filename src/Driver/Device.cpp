#include "stdafx.h"
#include "Device.h"
#include "nsvr_region.h"
#include "protobuff_defs/HighLevelEvent.pb.h"

Device::Device(
	std::string parentPlugin,
	DeviceDescriptor descriptor, 
	std::unique_ptr<DeviceVisualizer> visualizer,
	std::unique_ptr<BodyGraphCreator> bodygraph,
	std::unique_ptr<NodeDiscoverer> discoverer, 
	std::unique_ptr<PlaybackController> playback,
	std::unique_ptr<HapticInterface> haptics,
	std::unique_ptr<TrackingProvider> tracking
)
	: m_description(descriptor)
	, m_visualizer(std::move(visualizer))
	, m_bodygraph(std::move(bodygraph))
	, m_discoverer(std::move(discoverer))
	, m_playback(std::move(playback))
	, m_haptics(std::move(haptics))
	, m_originator(parentPlugin)
	, m_trackingProvider(std::move(tracking))
{
	//const auto& trackingNodes = m_discoverer->GetNodesOfType(nsvr_node_type_inertial_tracker);
	//for (nsvr_node_id node : trackingNodes) {
	//	m_trackingProvider->BeginStreaming(NodeId<local>{node});
	//}

}

void Device::DispatchEvent(const NullSpaceIPC::HighLevelEvent & event)
{
	switch (event.events_case()) {
	case NullSpaceIPC::HighLevelEvent::kPlaybackEvent:
		handlePlaybackEvent(event.parent_id(), event.playback_event());
		break;
	case NullSpaceIPC::HighLevelEvent::kLocationalEvent:
		handleLocationalEvent(event.parent_id(), event.locational_event());
	default:
		BOOST_LOG_TRIVIAL(info) << "[Device] Unrecognized request: " << event.events_case();
		break;
	}
}

void Device::DispatchEvent(const NullSpaceIPC::PlaybackEvent & playback_event)
{
}

void Device::DispatchEvent(uint64_t event_id, const NullSpaceIPC::SimpleHaptic & simple, const std::vector<nsvr_region>& regions)
{
	auto nodes = m_bodygraph->GetNodesAtRegions(regions);
	for (const auto& node : nodes) {
		m_haptics->SubmitSimpleHaptic(event_id, node, SimpleHaptic(simple.effect(), simple.duration(), simple.strength()));
	}
	std::vector<NodeId<local>> temp;
	for (const auto& n : nodes) { temp.emplace_back(n); }
	m_playback->CreateEventRecord(event_id, temp);
}

void Device::DispatchEvent(uint64_t event_id, const NullSpaceIPC::SimpleHaptic& simple, const std::vector<NodeId<local>>& nodes)
{
	//assumption: these nodes are haptic nodes.
	//This may not hold, so we should filter first. Need to add that method to the discoverer.

	for (const auto& node : nodes) {
		m_haptics->SubmitSimpleHaptic(event_id, node.value, SimpleHaptic(simple.effect(), simple.duration(), simple.strength()));
	}

	m_playback->CreateEventRecord(event_id, nodes); 
}

DeviceId<local> Device::id() const
{
	return DeviceId<local>{m_description.id};
}

std::string Device::name() const
{
	return m_description.displayName;
}


nsvr_device_concept Device::concept() const
{
	return m_description.concept;
}

std::string Device::parentPlugin() const
{
	return m_originator;
}

void Device::OnReceiveTrackingUpdate(TrackingHandler handler)
{
	m_trackingProvider->OnUpdate([this, callback = handler](NodeId<local> node, nsvr_quaternion* quat) {
		const auto& regions = m_bodygraph->GetRegionsForNode(node.value);
		for (const auto& region : regions) {
			callback(region, quat);
		}
	});
}

void Device::ForEachNode(std::function<void(Node*)> action)
{
	m_discoverer->ForEachNode(action);
}

void Device::update_visualizer(double dt)
{
	m_visualizer->simulate(dt);
}

std::vector<std::pair<nsvr_region, RenderedNode>> Device::render_visualizer()
{
	std::vector<std::pair<nsvr_region, RenderedNode>> taggedNodes;
	std::vector<RenderedNode> nodes = m_visualizer->render();
	for (const auto& node : nodes) {
		const auto& regions = m_bodygraph->GetRegionsForNode(node.Id.value);
		for (nsvr_region region : regions) {
			taggedNodes.emplace_back(region, node);
		}
	}
	return taggedNodes;
}

template<typename T, typename E>
std::vector<T> protoBufToVec(const google::protobuf::RepeatedField<E>& inArray) {
	std::vector<T> result;
	result.reserve(inArray.size());
	for (const auto& a : inArray) {
		result.push_back(static_cast<T>(a));
	}
	return result;
}

void Device::handleLocationalEvent(uint64_t event_id, const NullSpaceIPC::LocationalEvent & locational)
{
	switch (locational.events_case()) {
	case NullSpaceIPC::LocationalEvent::EventsCase::kSimpleHaptic:
		handleSimpleHaptic(event_id, locational.simple_haptic());
		break;
	default:
		break;
	}

}

void Device::handleSimpleHaptic(uint64_t event_id, const NullSpaceIPC::SimpleHaptic& simple)
{
	/*std::vector<nsvr_node_id> nodes;
	if (simple.where_case() == NullSpaceIPC::SimpleHaptic::kNodes) {
		nodes = protoBufToVec<nsvr_node_id>(simple.nodes().nodes());
	}
	else {
		auto regions = protoBufToVec<nsvr_region>(simple.regions().regions());
		nodes = m_bodygraph->GetNodesAtRegions(regions);
	}
	

	auto hapticNodes = m_discoverer->FilterByType(nodes, nsvr_node_type_haptic);

	for (nsvr_node_id node : hapticNodes) {
		m_haptics->SubmitSimpleHaptic(event_id, node, SimpleHaptic(simple.effect(), simple.duration(), simple.strength()));
	}

	m_playback->CreateEventRecord(event_id, hapticNodes);*/

}

void Device::handlePlaybackEvent(uint64_t id, const NullSpaceIPC::PlaybackEvent& playbackEvent)
{

	switch (playbackEvent.command()) {
	case NullSpaceIPC::PlaybackEvent_Command_UNPAUSE:
		m_playback->Resume(id);
		break;
	case NullSpaceIPC::PlaybackEvent_Command_PAUSE:
		m_playback->Pause(id);
		break;
	case NullSpaceIPC::PlaybackEvent_Command_CANCEL:
		m_playback->Cancel(id);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "[Device] Unknown playback event: " << playbackEvent.command();
		break;
	}
}
