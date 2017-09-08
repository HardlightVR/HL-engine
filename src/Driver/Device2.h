#pragma once

#include "NodeDiscoverer.h"
#include "BodyGraphCreator.h"
#include "DeviceContainer.h"
#include <memory>

#include <unordered_map>
#include <vector>

class Device2 {
public:
	Device2(DeviceDescriptor descriptor, std::unique_ptr<NodeDiscoverer>, std::unique_ptr<BodyGraphCreator>);
	void DispatchEvent(const NullSpaceIPC::HighLevelEvent& event);
	
private:
	DeviceDescriptor m_description;
	std::unique_ptr<NodeDiscoverer> m_discoverer;
	std::unique_ptr<BodyGraphCreator> m_bodygraph;
	std::unordered_map<uint64_t, std::vector<nsvr_node_id>> m_cachedRequests;
	void handleSimpleHaptic(uint64_t event_id, const NullSpaceIPC::SimpleHaptic& simple);
	void handlePlaybackEvent(uint64_t event_id, const NullSpaceIPC::PlaybackEvent& playbackEvent);
};