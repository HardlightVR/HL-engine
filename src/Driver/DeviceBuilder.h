#pragma once
#include <memory>
#include "BodyGraphCreator.h"
#include "NodeDiscoverer.h"
#include "PlaybackController.h"
#include "HapticInterface.h"
#include "TrackingProvider.h"
#include "Device.h"
#include "DeviceDescriptor.h"
#include "DeviceVisualizer.h"
#include "PluginApis.h"



#define DECLARE_FAKE_INTERFACE(name, api) \
struct name { \
virtual ~##name##() = default; \
virtual void Augment(##api##*) = 0;\
}; 


DECLARE_FAKE_INTERFACE(FakeWaveformHaptics, waveform_api)
DECLARE_FAKE_INTERFACE(FakeBufferedHaptics, buffered_api)
DECLARE_FAKE_INTERFACE(FakeNodeDiscoverer, device_api)
DECLARE_FAKE_INTERFACE(FakeBodygraph, bodygraph_api)
DECLARE_FAKE_INTERFACE(FakePlayback, playback_api)
DECLARE_FAKE_INTERFACE(FakeTracking, tracking_api)



struct DeviceLifetimeResources {
	std::unique_ptr<FakeBodygraph> bodygraph;
	std::unique_ptr<FakeNodeDiscoverer> discoverer;
	std::unique_ptr<FakeTracking> tracking;
	std::unique_ptr<FakePlayback> playback;
	std::unique_ptr<FakeWaveformHaptics> waveformHaptics;
	std::unique_ptr<FakeBufferedHaptics> bufferedHaptics;
	PluginApis apis;
};


class DeviceBuilder {
public:
	DeviceBuilder(PluginApis* apis);
	DeviceBuilder();

	DeviceBuilder& WithBodygraph(std::unique_ptr<FakeBodygraph> bodygraph);
	DeviceBuilder& WithNodeDiscoverer(std::unique_ptr<FakeNodeDiscoverer> discoverer);
	DeviceBuilder& WithTracking(std::unique_ptr<FakeTracking> tracking);
	DeviceBuilder& WithPlayback(std::unique_ptr<FakePlayback> playback);
	DeviceBuilder& WithWaveform(std::unique_ptr<FakeWaveformHaptics> haptics);
	DeviceBuilder& WithBuffered(std::unique_ptr<FakeBufferedHaptics> haptics);
	DeviceBuilder& WithDescriptor(DeviceDescriptor deviceDescription);
	DeviceBuilder& WithOriginatingPlugin(std::string pluginName);
	//DeviceBuilder& WithVisualizer(std::unique_ptr<DeviceVisualizer> visualizer);


	std::pair<std::unique_ptr<Device>, std::unique_ptr<DeviceLifetimeResources>> Build();
private:

	template<typename Api, typename HardwareBinding, typename FakeApi>
	std::unique_ptr<HardwareBinding> bind_api(std::unique_ptr<FakeApi>& fake);
	PluginApis* m_apis;
	
	std::unique_ptr<DeviceLifetimeResources> m_resources;

	std::unique_ptr<DeviceVisualizer> m_visualizer;

	boost::optional<DeviceDescriptor> m_description;
	boost::optional<std::string> m_originatingPlugin;

};

template<typename Api, typename HardwareBinding, typename FakeApi>
inline std::unique_ptr<HardwareBinding> DeviceBuilder::bind_api(std::unique_ptr<FakeApi>& fake)
{
	auto api = m_apis->GetApi<Api>();
	if (!api) {
		api = m_apis->ConstructDefault<Api>();
		fake->Augment(api);
	}

	return std::make_unique<HardwareBinding>(api);
	


	
}
