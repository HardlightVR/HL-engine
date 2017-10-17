#pragma once
#include <string>
#include <memory>

#include "Device.h"
#include "DeviceDescriptor.h" 
#include "PluginAPI.h" //needed for nsvr_device_id
#include "DeviceVisualizer.h"
#include "PluginInstance.h"
#include "logger.h"

class PluginApis;


class DeviceBuilder {
public:
	DeviceBuilder(PluginApis* apis, PluginInstance::DeviceResourceBundle& resources, nsvr_device_id id);

	
	DeviceBuilder& WithDescriptor(DeviceDescriptor deviceDescription);
	DeviceBuilder& WithOriginatingPlugin(std::string pluginName);
	//DeviceBuilder& WithVisualizer(std::unique_ptr<DeviceVisualizer> visualizer);
	DeviceBuilder& WithBodygraphDescriptor(Parsing::BodyGraphDescriptor descriptor);

	std::unique_ptr<Device> Build();
private:
	nsvr_device_id m_id;
	template<typename Api, typename HardwareBinding, typename FakeApi>
	std::unique_ptr<HardwareBinding> bind_component(std::unique_ptr<FakeApi>& fake);
	PluginApis* m_apis;
	
	PluginInstance::DeviceResourceBundle& m_resources;

	std::unique_ptr<DeviceVisualizer> m_visualizer;
	boost::optional<DeviceDescriptor> m_description;
	boost::optional<std::string> m_originatingPlugin;

};


template<typename Api, typename HardwareBinding, typename FakeApi>
inline std::unique_ptr<HardwareBinding> DeviceBuilder::bind_component(std::unique_ptr<FakeApi>& fake)
{
	auto api = m_apis->GetApi<Api>();
	if (!api) {
		api = m_apis->ConstructDefault<Api>();
		if (!fake) {
			BOOST_LOG_SEV(clogger::get(), nsvr_severity_warning) << "The device is missing api [" << Api::getApiType() << "] but there was no fake to take its place";
		}
		else {
			fake->Augment(api);
		}
	}

	return std::make_unique<HardwareBinding>(api);
}
