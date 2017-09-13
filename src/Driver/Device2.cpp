#include "stdafx.h"
#include "Device2.h"
#include "nsvr_region.h"

Device2::Device2(
	std::string parentPlugin,
	DeviceDescriptor descriptor, 
	std::unique_ptr<NodeDiscoverer> discoverer, 
	std::shared_ptr<BodyGraphCreator> bodygraph, 
	std::unique_ptr<PlaybackController> playback,
	std::unique_ptr<HapticInterface> haptics
)
	: m_description(descriptor)
	, m_discoverer(std::move(discoverer))
	, m_bodygraph(bodygraph)
	, m_playback(std::move(playback))
	, m_haptics(std::move(haptics))
	, m_originator(parentPlugin)
{
	
}

void Device2::DispatchEvent(const NullSpaceIPC::HighLevelEvent & event)
{
	switch (event.events_case()) {
	case NullSpaceIPC::HighLevelEvent::kSimpleHaptic:
		handleSimpleHaptic(event.parent_id(), event.simple_haptic());
		break;
	case NullSpaceIPC::HighLevelEvent::kPlaybackEvent:
		handlePlaybackEvent(event.parent_id(), event.playback_event());
		break;
	case NullSpaceIPC::HighLevelEvent::kRealtimeHaptic:
		//handleRealtimeEvent(event.parent_id(), event.realtime_haptic());
		//break;
	case NullSpaceIPC::HighLevelEvent::kCurveHaptic:
		//handleCurveHaptic(event.parent_id(), event.curve_haptic());
		//break;
	default:
		BOOST_LOG_TRIVIAL(info) << "[Device] Unrecognized request: " << event.events_case();
		break;
	}
}

nsvr_device_id Device2::id() const
{
	return m_description.id;
}

std::string Device2::name() const
{
	return m_description.displayName;
}


nsvr_device_concept Device2::concept() const
{
	return m_description.concept;
}

std::string Device2::parentPlugin() const
{
	return m_originator;
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

void Device2::handleSimpleHaptic(uint64_t event_id, const NullSpaceIPC::SimpleHaptic& simple)
{
	auto regions = protoBufToVec<nsvr_region>(simple.regions());

	auto allNodes = m_bodygraph->GetNodesAtRegions(regions);
	
	auto hapticNodes = m_discoverer->FilterByType(allNodes, nsvr_node_type_haptic);

	for (nsvr_node_id node : hapticNodes) {
		
		m_haptics->SubmitSimpleHaptic(event_id, node, SimpleHaptic(simple.effect(), simple.duration(), simple.strength()));
	}

	m_playback->CreateEventRecord(event_id, allNodes);

}

void Device2::handlePlaybackEvent(uint64_t id, const NullSpaceIPC::PlaybackEvent& playbackEvent)
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
