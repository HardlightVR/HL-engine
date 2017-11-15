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
#include "HardwareAnalogAudioInterface.h"


namespace NullSpaceIPC {
	class HighLevelEvent;
	class SimpleHaptic;
	class PlaybackEvent;
	class LocationalEvent;
	class SimpleHaptic;
	class BeginAnalogAudio;
	class EndAnalogAudio;
	class BufferedHaptic;
	class DeviceEvent;

}


class Device {
public:
	Device(
		std::string parentPlugin,
		DeviceDescriptor descriptor, 
		std::unique_ptr<DeviceVisualizer> visualizer,
		std::unique_ptr<HardwareBodygraphCreator>,
		std::unique_ptr<HardwareNodeEnumerator>
	);
	using TrackingHandler = std::function<void(nsvr_region, nsvr_quaternion*)>;

	void SetTracking(std::unique_ptr<HardwareTracking> tracking);
	void SetHaptics(std::unique_ptr<HardwareHapticInterface> haptics);
	void SetPlayback(std::unique_ptr<HardwarePlaybackController> playback);
	void SetAnalogAudio(std::unique_ptr<HardwareAnalogAudioInterface> analogAudio);


	DeviceId<local> id() const;



	void Deliver(const NullSpaceIPC::DeviceEvent& deviceEvent);
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

	void enableTracking();
	void disableTracking();


	void handle(uint64_t event_id, const NullSpaceIPC::PlaybackEvent& playbackEvent);

	//generate a bunch of wrappers that simply change NodeId<local> to nsvr_node_id while we are using them inconsistently..
	//should be able to remove the wrappers once we are internally consistent
	template<typename T>
	void handle(uint64_t event_id, const T& event, const std::vector<NodeId<local>>& targetNodes);


	//a temporary measure until we use NodeId<local> consistently?
	void handle(uint64_t eventId, const NullSpaceIPC::SimpleHaptic& event, const std::vector<nsvr_node_id>& targetNodes);
	void handle(uint64_t event_id, const NullSpaceIPC::BeginAnalogAudio& event, const std::vector<nsvr_node_id>& targetNodes);
	void handle(uint64_t event_id, const NullSpaceIPC::EndAnalogAudio& event, const std::vector<nsvr_node_id>& targetNodes);
	void handle(uint64_t event_id, const NullSpaceIPC::BufferedHaptic& event, const std::vector<nsvr_node_id>& targetNodes);

	std::string m_originator;
	DeviceDescriptor m_description;
	std::unique_ptr<DeviceVisualizer> m_visualizer;
	std::unique_ptr<HardwareBodygraphCreator> m_bodygraph;
	std::unique_ptr<HardwareNodeEnumerator> m_discoverer;
	std::unique_ptr<HardwarePlaybackController> m_playback;
	std::unique_ptr<HardwareHapticInterface> m_haptics;
	std::unique_ptr<HardwareTracking> m_trackingProvider;
	std::unique_ptr<HardwareAnalogAudioInterface> m_analogAudio;
	
};

std::vector<nsvr_node_id> toRawNodeIds(const std::vector<NodeId<local>>& targetNodes);

template<typename T>
inline void Device::handle(uint64_t event_id, const T & event, const std::vector<NodeId<local>>& targetNodes)
{
	handle(event_id, event, toRawNodeIds(targetNodes));
}
