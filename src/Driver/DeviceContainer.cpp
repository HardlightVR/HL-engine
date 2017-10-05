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

#include "DeviceBuilder.h"

DeviceContainer::DeviceContainer()

	: m_onDeviceAdded()
	, m_onDeviceRemoved()
	, m_devices()
	, m_simulations()
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
	//Need to have a good way of saying "Is this API available? Okay, here's this component. Else, here's some kind of stub, or fail to create the device,
	//or create a "simpler" version of the device. We can simply check if the GetApi call returns nullptr, but there's no machinery right now
	//to deal with that.
	
	

	auto playback = std::make_unique<HardwarePlaybackController>(apis.GetApi<playback_api>());

	auto bodygraph = std::make_shared<HardwareBodygraphCreator>(bodyGraphDescriptor, apis.GetApi<bodygraph_api>());

	auto nodes = std::make_unique<HardwareNodeEnumerator>(desc.id, apis.GetApi<device_api>());
	
	auto tracking = std::make_unique<HardwareTracking>(apis.GetApi<tracking_api>());

	auto haptics = std::make_unique<HardwareHapticInterface>(apis.GetApi<buffered_api>(), apis.GetApi<waveform_api>());

	m_deviceLock.lock();

	m_devices.push_back(std::make_unique<Device>(originatingPlugin, desc, bodygraph, std::move(nodes), std::move(playback), std::move(haptics), std::move(tracking)));
	m_simulations.push_back(std::make_unique<SimulatedDevice>(desc.id, desc.displayName, originatingPlugin,  apis, bodygraph));

	m_deviceLock.unlock();

	//this actually won't work as intended
	//it should grab the pointer inside the locked regionx
	//todo(fix)
	m_onDeviceAdded(m_devices.back().get());
}




void DeviceContainer::RemoveDevice(DeviceId<local> id, std::string pluginName)
{
	m_deviceLock.lock();

	auto it = std::find_if(m_devices.begin(), m_devices.end(), [id, pluginName](const auto& device) { return device->id() == id && device->parentPlugin() == pluginName; });
	if (it != m_devices.end()) {
		m_onDeviceRemoved(it->get());
		m_devices.erase(it);
		auto sim = std::find_if(m_simulations.begin(), m_simulations.end(), [id, pluginName](const auto& device) { return device->id() == id && device->originatingPlugin() == pluginName; });
		m_simulations.erase(sim);
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

void DeviceContainer::EachSimulation(SimFn action) {
	std::lock_guard<std::mutex> guard(m_deviceLock);
	for (auto& ptr : m_simulations) {
		action(ptr.get());
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


