#include "stdafx.h"
#include "DeviceBuilder.h"
#include "PluginApis.h"



//Either we are provided a set of apis from the plugin (which we copy)
DeviceBuilder::DeviceBuilder(PluginApis* apis, PluginInstance::DeviceResources* resources, nsvr_device_id id) : m_id(id), m_apis(apis), m_resources(resources)
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




std::unique_ptr<Device> DeviceBuilder::Build()
{
	


	auto waveformApi = bind_component<waveform_api, HardwareWaveform>(m_resources->waveformHaptics);

	auto playback = bind_component<playback_api, HardwarePlaybackController>(m_resources->playback);

	auto tracking = bind_component<tracking_api, HardwareTracking>(m_resources->tracking);

	auto discovery = bind_component<device_api, HardwareNodeEnumerator>(m_resources->discoverer);
	discovery->setId(m_description->id);

	auto bodygraph = bind_component<bodygraph_api, HardwareBodygraphCreator>(m_resources->bodygraph);

	if (m_resources->bodygraphDescriptor) {
		bodygraph->provideDescription(*m_resources->bodygraphDescriptor);
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
