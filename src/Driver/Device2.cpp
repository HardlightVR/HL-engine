#include "stdafx.h"
#include "Device2.h"
#include "nsvr_region.h"
Device2::Device2(DeviceDescriptor descriptor, std::unique_ptr<NodeDiscoverer> discoverer, std::unique_ptr<BodyGraphCreator> bodygraph)
	: m_description(descriptor)
	, m_discoverer(std::move(discoverer))
	, m_bodygraph(std::move(bodygraph))
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

void Device2::handleSimpleHaptic(uint64_t event_id, const NullSpaceIPC::SimpleHaptic& simple)
{
	for (const auto& proto_region : simple.regions()) {
		nsvr_region region = static_cast<nsvr_region>(proto_region);

		m_bodygraph->ForEachNodeAtRegion(region, [this](nsvr_node_id id) {
			Node* node = m_discoverer->Get(id);
			if (node->type() == nsvr_node_type_haptic) {
				//submitWaveform(id, Waveform{});
			}
		});
	}
}

void Device2::handlePlaybackEvent(uint64_t id, const NullSpaceIPC::PlaybackEvent& playbackEvent)
{
	switch (playbackEvent.command()) {
	case NullSpaceIPC::PlaybackEvent_Command_UNPAUSE:
		//api->submit_unpause(id);
		break;
	case NullSpaceIPC::PlaybackEvent_Command_PAUSE:
		//api->submit_pause(id);
		break;
	case NullSpaceIPC::PlaybackEvent_Command_CANCEL:
		//api->submit_cancel(id);
		break;
	default:
	//	BOOST_LOG_TRIVIAL(warning) << "[Device] Unknown playback event: " << event.command();
		break;
	}

	m_discoverer->ForEachNode([](Node* node) {
		//submitPlaybackCommand(node->id, command) //for simulation purposes
	});
}
