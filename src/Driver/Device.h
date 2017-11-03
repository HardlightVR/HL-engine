#pragma once
#include <stdint.h>
#include <memory>
#include <string>

#include "DeviceDescriptor.h"
#include "DeviceIds.h"
#include "DeviceVisualizer.h"
#include "HardwareBodygraphCreator.h"
#include "HardwareNodeEnumerator.h"
#include "HardwarePlaybackController.h"
#include "HardwareHapticInterface.h"
#include "HardwareTracking.h"


namespace NullSpaceIPC {
	class HighLevelEvent;
	class SimpleHaptic;
	class PlaybackEvent;
	class LocationalEvent;
	class SimpleHaptic;
	class ContinuousHaptic;

}


class Device {
public:
	Device(
		std::string parentPlugin,
		DeviceDescriptor descriptor, 
		std::unique_ptr<DeviceVisualizer> visualizer,
		std::unique_ptr<HardwareBodygraphCreator>,
		std::unique_ptr<HardwareNodeEnumerator>, 
		std::unique_ptr<HardwarePlaybackController>, 
		std::unique_ptr<HardwareHapticInterface>,
		std::unique_ptr<HardwareTracking>
	);
	using TrackingHandler = std::function<void(nsvr_region, nsvr_quaternion*)>;


	

	DeviceId<local> id() const;



	
	void Deliver(uint64_t eventId, const NullSpaceIPC::LocationalEvent&, const std::vector<nsvr_region>& regions);
	void Deliver(uint64_t eventId, const NullSpaceIPC::LocationalEvent&, const::std::vector<NodeId<local>>& nodes);
	void Deliver(uint64_t eventId, const NullSpaceIPC::PlaybackEvent&);

	

	std::string name() const;
	nsvr_device_concept concept() const;
	std::string parentPlugin() const;
	void OnReceiveTrackingUpdate(TrackingHandler handler);
	void ForEachNode(std::function<void(Node*)> action);
	void UpdateVisualizer(double dt);
	std::vector<std::pair<nsvr_region, RenderedNode>> RenderVisualizer();
private:
	void handle(uint64_t eventId, const NullSpaceIPC::ContinuousHaptic& event, const std::vector<NodeId<local>>& targetNodes);
	void handle(uint64_t eventId, const NullSpaceIPC::SimpleHaptic& event, const std::vector<NodeId<local>>& targetNodes);
	void handle(uint64_t event_id, const NullSpaceIPC::PlaybackEvent& playbackEvent);

	//a temporary measure until we use NodeId<local> consistently?
	void handle(uint64_t eventId, const NullSpaceIPC::ContinuousHaptic& event, const std::vector<nsvr_node_id>& targetNodes);
	void handle(uint64_t eventId, const NullSpaceIPC::SimpleHaptic& event, const std::vector<nsvr_node_id>& targetNodes);

	std::string m_originator;
	DeviceDescriptor m_description;
	std::unique_ptr<DeviceVisualizer> m_visualizer;
	std::unique_ptr<HardwareBodygraphCreator> m_bodygraph;
	std::unique_ptr<HardwareNodeEnumerator> m_discoverer;
	std::unique_ptr<HardwarePlaybackController> m_playback;
	std::unique_ptr<HardwareHapticInterface> m_haptics;
	std::unique_ptr<HardwareTracking> m_trackingProvider;
	
};

