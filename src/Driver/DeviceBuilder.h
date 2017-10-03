#pragma once
#include <memory>
#include "BodyGraphCreator.h"
#include "NodeDiscoverer.h"
#include "PlaybackController.h"
#include "HapticInterface.h"
#include "TrackingProvider.h"
#include "Device.h"
#include "DeviceDescriptor.h"
class DeviceBuilder {
public:
	DeviceBuilder();

	DeviceBuilder& WithDefaultBodygraph();
	DeviceBuilder& WithDefaultNodeDiscovery();
	DeviceBuilder& WithDefaultTracking();
	DeviceBuilder& WithDefaultHaptics();
	DeviceBuilder& WithDefaultPlaybackControl();
	DeviceBuilder& WithDefaultDescription();
	DeviceBuilder& WithDefaultOriginatingPlugin();

	DeviceBuilder& WithBodygraph(std::unique_ptr<BodyGraphCreator> bodygraph);
	DeviceBuilder& WithNodeDiscoverer(std::unique_ptr<NodeDiscoverer> discoverer);
	DeviceBuilder& WithTracking(std::unique_ptr<TrackingProvider> tracking);
	DeviceBuilder& WithPlayback(std::unique_ptr<PlaybackController> playback);
	DeviceBuilder& WithHapticInterface(std::unique_ptr<HapticInterface> haptics);
	DeviceBuilder& WithDescriptor(DeviceDescriptor deviceDescription);
	DeviceBuilder& WithOriginatingPlugin(std::string pluginName);



	std::unique_ptr<Device> Build();
private:
	std::shared_ptr<BodyGraphCreator> m_bodygraph;
	std::unique_ptr<NodeDiscoverer> m_discoverer;
	std::unique_ptr<TrackingProvider> m_tracking;
	std::unique_ptr<PlaybackController> m_playback;
	std::unique_ptr<HapticInterface> m_haptics;
	boost::optional<DeviceDescriptor> m_description;
	boost::optional<std::string> m_originatingPlugin;

};