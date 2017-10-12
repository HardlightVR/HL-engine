#include "stdafx.h"
#include "DeviceBuilder.h"
#include "FakeBodygraph.h"
#include "HardwareNodeEnumerator.h"
#include "FakeDiscoverer.h"
#include "HardwareTracking.h"
#include "FakeTrackingProvider.h"
#include "SharedTypes.h"


//Temporarily sticking this in here until I know how the fake will work
class FakePlayback  {
public:
	void Augment(playback_api* api);
	
};

class FakeHaptics : public HapticInterface {
public: 
	void Augment(waveform_api* api);
	void Augment(buffered_api* api);
	void SubmitSimpleHaptic(uint64_t request_id, nsvr_node_id id, SimpleHaptic data) override {}
};


//template<typename Api, typename Initializer>
//void default_initialize(PluginApis* apis) {
//	if (!apis->GetApi<Api>()) {
//		apis->RegisterDefault<Api>();
//		Initializer().Augment(apis->GetApi<Api>());
//	}
//}


//Either we are provided a set of apis from the plugin (which we copy)
DeviceBuilder::DeviceBuilder(PluginApis* apis) : m_apis(*apis)
{
	PluginApis p;
	p.ConstructDefault<device_api>();

}

//Or we are on our own to build it
DeviceBuilder::DeviceBuilder() : m_apis()
{
	//Need to be careful because the default *_apis will just crash because they have null callbacks. Perhaps we should
	//assign them to do nothing?

//	m_apis.RegisterInternal(Apis::Device, std::make_unique<device_api>());
}

DeviceBuilder & DeviceBuilder::WithDefaultBodygraph()
{
	//m_bodygraph = FakeBodygraphBuilder().Build();
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDefaultNodeDiscovery()
{
	//if (!m_description) {
	//	WithDefaultDescription();
	//}

	//assert(m_apis.GetApi<device_api>());

	//m_discoverer = std::make_unique<HardwareNodeEnumerator>(m_description->id, m_apis.GetApi<device_api>());
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDefaultTracking()
{
	//m_tracking = std::make_unique<HardwareTracking>(m_apis.GetApi<tracking_api>());
	return *this;
}

DeviceBuilder& DeviceBuilder::WithDefaultHaptics()
{
	//m_haptics = std::make_unique<FakeHaptics>();
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDefaultPlaybackControl()
{
	//m_playback = std::make_unique<FakePlayback>();
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDefaultDescription()
{
	DeviceDescriptor desc;
	desc.concept = nsvr_device_concept_controller;
	desc.displayName = "Generic Controller Device";
	desc.id = 0;
	m_description = desc;
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDefaultOriginatingPlugin()
{
	m_originatingPlugin = "Fake Plugin";
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDefaultVisualizer()
{
	//m_visualizer = std::make_unique<DeviceVisualizer>();

	return *this;
}

DeviceBuilder & DeviceBuilder::WithBodygraph(std::unique_ptr<BodyGraphCreator> bodygraph)
{
//	m_bodygraph = std::move(bodygraph);
	return *this;
}

DeviceBuilder & DeviceBuilder::WithNodeDiscoverer(std::unique_ptr<NodeDiscoverer> discoverer)
{
	//m_discoverer = std::move(discoverer);
	return *this;
}

DeviceBuilder & DeviceBuilder::WithTracking(std::unique_ptr<TrackingProvider> tracking)
{
//	m_tracking = std::move(tracking);
	return *this;
}

DeviceBuilder & DeviceBuilder::WithPlayback(std::unique_ptr<PlaybackController> playback)
{
	//m_playback = std::move(playback);
	return *this;
}

DeviceBuilder & DeviceBuilder::WithHapticInterface(std::unique_ptr<HapticInterface> haptics)
{
	//m_haptics = std::move(haptics);
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDescriptor(DeviceDescriptor deviceDescription)
{
	m_description = deviceDescription;
	return *this;
}

DeviceBuilder & DeviceBuilder::WithOriginatingPlugin(std::string pluginName)
{
	m_originatingPlugin = pluginName;
	return *this;
}

DeviceBuilder & DeviceBuilder::WithVisualizer(std::unique_ptr<DeviceVisualizer> visualizer)
{
//	m_visualizer = std::move(visualizer);
	return *this;
}

std::unique_ptr<Device> DeviceBuilder::Build()
{
	if (!m_description) {
		WithDefaultDescription();
	}

	if (!m_originatingPlugin) {
		WithDefaultOriginatingPlugin();
	}

	if (!m_bodygraph) {
		WithDefaultBodygraph();
	}

	if (!m_discoverer) {
		WithDefaultNodeDiscovery();
	}

	if (!m_tracking) {
		WithDefaultTracking();
	}

	if (!m_playback) {
		WithDefaultPlaybackControl();
	}

	

	if (!m_visualizer) {
		WithDefaultVisualizer();
	}

	if (!m_haptics) {
		WithDefaultHaptics();
	}


	return std::make_unique<Device>(
		*m_originatingPlugin,
		*m_description,
		std::move(m_visualizer),
		std::move(m_bodygraph),
		std::move(m_discoverer),
		std::move(m_playback),
		std::move(m_haptics),
		std::move(m_tracking)
	);
	
}

void FakePlayback::Augment(playback_api * api)
{
	api->submit_cancel.handler = [](auto a, auto b, auto c) {};
	api->submit_pause.handler = [](auto a, auto b, auto c) {};
	api->submit_unpause.handler = [](auto a, auto b, auto c) {};
}

void FakeHaptics::Augment(waveform_api * api)
{
	api->submit_activate.handler = [](auto a, auto b, auto d, auto e) {};
}

void FakeHaptics::Augment(buffered_api * api)
{
	api->submit_buffer.handler = [](auto a, auto b, auto c, auto d, auto e) {};
	api->submit_getmaxsamples.handler = [](auto a, auto b) {};
	api->submit_getsampleduration.handler = [](auto a, auto b) {};
}
