#include "stdafx.h"
#include "DeviceBuilder.h"
#include "FakeBodygraph.h"
#include "HardwareNodeEnumerator.h"
#include "FakeDiscoverer.h"
#include "HardwareTracking.h"
#include "FakeTrackingProvider.h"
#include "SharedTypes.h"
#include "HardwareHapticInterface.h"
#include "FakeResources.h"



//Either we are provided a set of apis from the plugin (which we copy)
DeviceBuilder::DeviceBuilder(PluginApis* apis, PluginInstance::DeviceResourceBundle& resources, nsvr_device_id id) : m_id(id), m_apis(apis), m_resources(resources)
{
	//PluginApis p;
	//p.ConstructDefault<device_api>();

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

DeviceBuilder & DeviceBuilder::WithBodygraphDescriptor(Parsing::BodyGraphDescriptor descriptor)
{
	m_bgDescriptor = descriptor;
	return *this;
}



std::unique_ptr<Device> DeviceBuilder::Build()
{
	


	auto waveformApi = bind_component<waveform_api, HardwareWaveform>(m_resources->waveformHaptics);

	auto playback = bind_component<playback_api, HardwarePlaybackController>(m_resources->playback);

	auto tracking = bind_component<tracking_api, HardwareTracking>(m_resources->tracking);

	auto discovery = bind_component<device_api, HardwareNodeEnumerator>(m_resources->discoverer);
	discovery->setId(m_description->id);

	auto bodygraph = bind_component<bodygraph_api, HardwareBodygraphCreator>(m_resources->bodygraph);

	if (m_bgDescriptor) {
		bodygraph->provideDescription(*m_bgDescriptor);
	}



	m_visualizer = std::make_unique<DeviceVisualizer>();
	m_visualizer->provideApi(m_apis->GetApi<playback_api>());
	m_visualizer->provideApi(m_apis->GetApi<waveform_api>());


	//need to provide buffered api


	return std::make_unique<Device>(
		*m_originatingPlugin,
		*m_description,
		std::move(m_visualizer),
		std::move(bodygraph),
		std::move(discovery),
		std::move(playback),
		std::move(waveformApi),
		std::move(tracking)
		);

	
}
//
//void FakePlayback::Augment(playback_api * api)
//{
//	api->submit_cancel.handler = [](auto a, auto b, auto c) {};
//	api->submit_pause.handler = [](auto a, auto b, auto c) {};
//	api->submit_unpause.handler = [](auto a, auto b, auto c) {};
//}
//
//void FakeWaveformHaptics::Augment(waveform_api * api)
//{
//	api->submit_activate.handler = [](auto a, auto b, auto d, auto e) {};
//}
//
