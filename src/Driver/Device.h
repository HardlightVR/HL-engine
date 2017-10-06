#pragma once
#include <memory>
#include <string>
#include "NodeDiscoverer.h"
#include "BodyGraphCreator.h"
#include "PlaybackController.h"
#include "DeviceDescriptor.h"
#include "HapticInterface.h"
#include "TrackingProvider.h"
#include "DeviceIds.h"
#include "DeviceVisualizer.h"
//theory:
//NodeDiscoverer can be mocked out. So we can provide a bunch of fake nodes
//I think PlaybackController can be a concrete class and can get rid of the interface.

// BodyGraphCreator can be mocked out, but must be shared between the real device and a simulated device either way, so the simulation knows
//which nodes belong in which regions. 
//Then if we want to create an entirely fake device with no hardware present,
//we basically need to build out a skeleton of PluginApis that just has stubs for most of the apis, but e.g. returns the desired device name

// Then the function calls into the underlying apis will do nothing, but will also be replicated through the "instrumentation" pointer into simulated
// functionality. 

namespace NullSpaceIPC {
	class HighLevelEvent;
	class SimpleHaptic;
	class PlaybackEvent;
	class LocationalEvent;
}
class Device {
public:
	Device(
		std::string parentPlugin,
		DeviceDescriptor descriptor, 
		std::unique_ptr<DeviceVisualizer> visualizer,
		std::unique_ptr<BodyGraphCreator>,
		std::unique_ptr<NodeDiscoverer>, 
		std::unique_ptr<PlaybackController>, 
		std::unique_ptr<HapticInterface>,
		std::unique_ptr<TrackingProvider>
	);
	using TrackingHandler = std::function<void(nsvr_region, nsvr_quaternion*)>;

	void DispatchEvent(const NullSpaceIPC::HighLevelEvent& event);
	void DispatchEvent(const NullSpaceIPC::PlaybackEvent& playback_event);
	void DispatchEvent(uint64_t event_id, const NullSpaceIPC::SimpleHaptic& haptic_event, const std::vector<NodeId<local>>& nodes);
	void DispatchEvent(uint64_t event_id, const NullSpaceIPC::SimpleHaptic& haptic_event, const std::vector<nsvr_region>& regions);
	DeviceId<local> id() const;
	std::string name() const;
	nsvr_device_concept concept() const;
	std::string parentPlugin() const;
	void OnReceiveTrackingUpdate(TrackingHandler handler);
	void ForEachNode(std::function<void(Node*)> action);
	void update_visualizer(double dt);
	std::vector<std::pair<nsvr_region, RenderedNode>> render_visualizer();
private:
	std::string m_originator;
	DeviceDescriptor m_description;
	std::unique_ptr<DeviceVisualizer> m_visualizer;
	std::unique_ptr<BodyGraphCreator> m_bodygraph;
	std::unique_ptr<NodeDiscoverer> m_discoverer;
	std::unique_ptr<PlaybackController> m_playback;
	std::unique_ptr<HapticInterface> m_haptics;
	std::unique_ptr<TrackingProvider> m_trackingProvider;
	void handleLocationalEvent(uint64_t event_id, const NullSpaceIPC::LocationalEvent& locational);
	void handleSimpleHaptic(uint64_t event_id, const NullSpaceIPC::SimpleHaptic& simple);
	void handlePlaybackEvent(uint64_t event_id, const NullSpaceIPC::PlaybackEvent& playbackEvent);
};