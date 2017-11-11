#include "stdafx.h"
#include "DeviceBuilder.h"
#include "PluginApis.h"
#include "DeviceVisualizer.h"

#include "HardwareHapticInterface.h"
#include "HardwarePlaybackController.h"
#include "HardwareNodeEnumerator.h"
#include "HardwareBodygraphCreator.h"
#include "HardwareTracking.h"
#include "HardwareAnalogAudioInterface.h"

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
	//Bad symmetry here between the member functions augment_with and bind_component. Maybe augment_with and bind_component should be free functions..
	//Either way, they need better names
	//Augment_with_fake basically default constructs the api if not present (so that calls into that api will be no-ops) and then
	//if the fake is present, hooks the fake into that newly constructed API.

	//bind_component is a straight up old name, but it does almost the same as augmnet_with_fake except it also constructs a unique ptr 
	//to the particular interface type (only supports constructor with one argument: the underlying api.)
	

	//Reasons why we can't just pass in GetApi<buffered> and GetApi<waveform> to HardwareHapticInterface: although it would be a natural way of saying
	//"this api is present" (non-nullptr) or "this api is not present" (nullptr) then we can't make a good fake, because it will be nullptr in the case
	//of a virtual device. Really, we want to say "hey this api is not present ONLY in these cases: 1) not present in a real plugin 2) not present in a virtual plugin


	auto hapticInterface = std::make_unique<HardwareHapticInterface>();

	if (augment_with_fake<waveform_api>(m_resources->waveformHaptics)) {
		hapticInterface->ProvideWaveform(m_apis->GetApi<waveform_api>());
	}

	if (augment_with_fake<buffered_api>(m_resources->bufferedHaptics)) {
		hapticInterface->ProvideBuffered(m_apis->GetApi<buffered_api>());
	}

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
	m_visualizer->provideApi(m_apis->GetApi<buffered_api>());

	//todo: provide visualizer with audio mode so we can do a sine wave or something
	//need to provide buffered api


	auto device =  std::make_unique<Device>(
		*m_originatingPlugin,
		*m_description,
		std::move(m_visualizer),
		std::move(bodygraph),
		std::move(discovery)
	);

	device->SetHaptics(std::move(hapticInterface));
	device->SetPlayback(std::move(playback));
	device->SetTracking(std::move(tracking));

	//vvv this is how it should be done
	if (m_apis->GetApi<analogaudio_api>()) {
		device->SetAnalogAudio(std::make_unique<HardwareAnalogAudioInterface>(m_apis->GetApi<analogaudio_api>()));
	}

	return device;
	
}
