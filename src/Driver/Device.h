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
		std::unique_ptr<HardwareWaveform>,
		std::unique_ptr<HardwareTracking>
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
	std::unique_ptr<HardwareBodygraphCreator> m_bodygraph;
	std::unique_ptr<HardwareNodeEnumerator> m_discoverer;
	std::unique_ptr<HardwarePlaybackController> m_playback;
	std::unique_ptr<HardwareWaveform> m_haptics;
	std::unique_ptr<HardwareTracking> m_trackingProvider;
	void handleLocationalEvent(uint64_t event_id, const NullSpaceIPC::LocationalEvent& locational);
	void handleSimpleHaptic(uint64_t event_id, const NullSpaceIPC::SimpleHaptic& simple);
	void handlePlaybackEvent(uint64_t event_id, const NullSpaceIPC::PlaybackEvent& playbackEvent);
};