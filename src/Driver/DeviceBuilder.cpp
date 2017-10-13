#include "stdafx.h"
#include "DeviceBuilder.h"
#include "FakeBodygraph.h"
#include "HardwareNodeEnumerator.h"
#include "FakeDiscoverer.h"
#include "HardwareTracking.h"
#include "FakeTrackingProvider.h"
#include "SharedTypes.h"
#include "HardwareHapticInterface.h"




struct DefaultFakeWaveformHaptics : public FakeWaveformHaptics {
	void Augment(waveform_api* api) override {}
};


struct DefaultFakeBufferedHaptics : public FakeBufferedHaptics {
	void Augment(buffered_api* api) override {}
};



//Either we are provided a set of apis from the plugin (which we copy)
DeviceBuilder::DeviceBuilder(PluginApis* apis) : m_apis(apis)
{
	//PluginApis p;
	//p.ConstructDefault<device_api>();

}

//Or we are on our own to build it
DeviceBuilder::DeviceBuilder() 
{

	m_resources->apis = PluginApis();
	m_apis = &m_resources->apis;

}



std::pair<std::unique_ptr<Device>, std::unique_ptr<DeviceLifetimeResources>> DeviceBuilder::Build()
{

	if (!m_originatingPlugin) {
		m_originatingPlugin = "Fake Plugin";
	}

	if (!m_description) {
		DeviceDescriptor desc;
		desc.concept = nsvr_device_concept_controller;
		desc.displayName = "Generic Controller Device";
		desc.id = 0;
		m_description = desc;
	}

	if (!m_resources->waveformHaptics) {
		m_resources->waveformHaptics = std::make_unique<DefaultFakeWaveformHaptics>();
	}
	
	if (!m_resources->bufferedHaptics) {
		m_resources->bufferedHaptics = std::make_unique<DefaultFakeBufferedHaptics>();
	}
	
	auto waveformApi = bind_api<waveform_api, HardwareWaveform>(m_resources->waveformHaptics);

	auto playback = bind_api<playback_api, HardwarePlaybackController>(m_resources->playback);

	auto tracking = bind_api<tracking_api, HardwareTracking>(m_resources->tracking);

	auto discovery = bind_api<device_api, HardwareNodeEnumerator>(m_resources->discoverer);
	discovery->setId(m_description->id);

	auto bodygraph = bind_api<bodygraph_api, HardwareBodygraphCreator>(m_resources->bodygraph);


	m_visualizer = std::make_unique<DeviceVisualizer>();
	m_visualizer->provideApi(m_apis->GetApi<playback_api>());
	//need to provide buffered api


	//note that this will break as soon as the device builder is destroyed, because m_waveformhaptics will be destroyed, which is the fake.
	//need a way to prolong its life.
	//It works for a real plugin because the plugin stays alive in the manager and outlives any of the Device objects. Here, the builder may only be used 
	//momentarily. 


	auto device = std::make_unique<Device>(
		*m_originatingPlugin,
		*m_description,
		std::move(m_visualizer),
		std::move(bodygraph),
		std::move(discovery),
		std::move(playback),
		std::move(waveformApi),
		std::move(tracking)
	);


	auto pair = std::make_pair(std::move(device), std::move(m_resources));
	return std::move(pair);

	
}

void FakePlayback::Augment(playback_api * api)
{
	api->submit_cancel.handler = [](auto a, auto b, auto c) {};
	api->submit_pause.handler = [](auto a, auto b, auto c) {};
	api->submit_unpause.handler = [](auto a, auto b, auto c) {};
}

void FakeWaveformHaptics::Augment(waveform_api * api)
{
	api->submit_activate.handler = [](auto a, auto b, auto d, auto e) {};
}

//void FakeWaveformHaptics::Augment(buffered_api * api)
//{
//	api->submit_buffer.handler = [](auto a, auto b, auto c, auto d, auto e) {};
//	api->submit_getmaxsamples.handler = [](auto a, auto b) {};
//	api->submit_getsampleduration.handler = [](auto a, auto b) {};
//}

DeviceBuilder & DeviceBuilder::WithBodygraph(std::unique_ptr<FakeBodygraph> bodygraph)
{
	m_resources->bodygraph = std::move(bodygraph);
	return *this;
}

DeviceBuilder & DeviceBuilder::WithNodeDiscoverer(std::unique_ptr<FakeNodeDiscoverer> discoverer)
{
	m_resources->discoverer = std::move(discoverer);
	return *this;
}
