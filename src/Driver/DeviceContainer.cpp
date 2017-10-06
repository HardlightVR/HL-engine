#include "stdafx.h"
#include "DeviceContainer.h"

#include <experimental/vector>
#include "HardwareBodygraphCreator.h"
#include "HardwareNodeEnumerator.h"
#include "HardwarePlaybackController.h"
#include "HardwareHapticInterface.h"
#include "HardwareTracking.h"
#include "HapticInterface.h"
#include "DriverMessenger.h"
#include "DeviceIds.h"
#include "DeviceVisualizer.h"

#include "DeviceBuilder.h"

#include "logger.h"
DeviceContainer::DeviceContainer()

	: m_onDeviceAdded()
	, m_onDeviceRemoved()
	, m_devices()
	, m_deviceAddedSubs()
	, m_deviceRemovedSubs()
	, m_deviceLock()
{
}

void DeviceContainer::AddDevice(nsvr_device_id id, PluginApis & apis, Parsing::BodyGraphDescriptor bodyGraphDescriptor, std::string originatingPlugin)
{
	if (auto api = apis.GetApi<device_api>()) {

		nsvr_device_ids ids = { 0 };

		api->submit_enumeratedevices(&ids);

		for (std::size_t i = 0; i < ids.device_count; i++) {

			nsvr_device_info info = { 0 };
			api->submit_getdeviceinfo(ids.ids[i], &info);

			DeviceDescriptor desc;
			desc.displayName = std::string(info.name);
			desc.id = info.id;
			desc.concept = info.concept;
			addDevice(desc, apis, std::move(bodyGraphDescriptor), originatingPlugin);
		}
	}

}



void DeviceContainer::AddDevice(nsvr_device_id, std::unique_ptr<Device> device)
{
	m_devices.push_back(std::move(device));
	m_onDeviceAdded(m_devices.back().get());
}



void DeviceContainer::addDevice(const DeviceDescriptor& desc, PluginApis& apis, Parsing::BodyGraphDescriptor bodyGraphDescriptor, std::string originatingPlugin)
{

	std::vector<Apis> required_apis = { Apis::Device };

	for (Apis required : required_apis) {
		if (!apis.Supports(required)) {
			BOOST_LOG_SEV(clogger::get(), nsvr_severity_error) << originatingPlugin << " tried to create a device, but does not support required api: " << required._to_string();
			return;
		}
	}


	auto visualizer = std::make_unique<DeviceVisualizer>();

	if (apis.Supports<playback_api>()) {
		visualizer->provideApi(apis.GetApi<playback_api>());
	} 
	if (apis.Supports<waveform_api>()) {
		visualizer->provideApi(apis.GetApi<waveform_api>());
	}
	if (apis.Supports<buffered_api>()) {
		//todo
	}


	DeviceBuilder builder;
	
	builder
		.WithDescriptor(desc)
		.WithOriginatingPlugin(originatingPlugin)
		.WithNodeDiscoverer(std::make_unique<HardwareNodeEnumerator>(desc.id, apis.GetApi<device_api>()))
		.WithVisualizer(std::move(visualizer));


	//The following are optional apis.
	if (apis.Supports<playback_api>()) {
		builder.WithPlayback(std::make_unique<HardwarePlaybackController>(apis.GetApi<playback_api>()));
	}

	if (apis.Supports<bodygraph_api>()) {
		builder.WithBodygraph(std::make_unique<HardwareBodygraphCreator>(bodyGraphDescriptor, apis.GetApi<bodygraph_api>()));
	}

	if (apis.Supports<tracking_api>()) {
		builder.WithTracking(std::make_unique<HardwareTracking>(apis.GetApi<tracking_api>()));
	}

	if (apis.Supports<buffered_api>() || apis.Supports<waveform_api>()){
		builder.WithHapticInterface(std::make_unique<HardwareHapticInterface>(apis.GetApi<buffered_api>(), apis.GetApi<waveform_api>()));
	}
	

	m_deviceLock.lock();
	m_devices.push_back(builder.Build());
	Device* newlyAdded = m_devices.back().get();
	m_deviceLock.unlock();

	m_onDeviceAdded(newlyAdded);
}




void DeviceContainer::RemoveDevice(DeviceId<local> id, std::string pluginName)
{
	m_deviceLock.lock();

	auto it = std::find_if(m_devices.begin(), m_devices.end(), [id, pluginName](const auto& device) { return device->id() == id && device->parentPlugin() == pluginName; });
	if (it != m_devices.end()) {
		m_onDeviceRemoved(it->get());
		m_devices.erase(it);
	}

	m_deviceLock.unlock();


}


void DeviceContainer::EachDevice(DeviceFn forEach)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);
	for (auto& ptr : m_devices) {
		forEach(ptr.get());
	}
}



Device* DeviceContainer::Get(std::string plugin, DeviceId<local> id)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);

	auto it = std::find_if(m_devices.begin(), m_devices.end(), [id, plugin](const auto& device) { return device->id() == id && device->parentPlugin() == plugin; });
	if (it != m_devices.end()) {
		return (*it).get();
	}
	else {
		return nullptr;
	}
}

void DeviceContainer::OnDeviceAdded(DeviceEvent::slot_type slot)
{
	m_onDeviceAdded.connect(slot);
}

void DeviceContainer::OnDeviceRemoved(DeviceEvent::slot_type slot)
{
	m_onDeviceRemoved.connect(slot);
}


