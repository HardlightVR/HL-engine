#include "stdafx.h"
#include "DeviceContainer.h"
#include <experimental/vector>

#include "Device2.h"

#include "HardwareBodygraphCreator.h"
#include "HardwareNodeEnumerator.h"
#include "HardwarePlaybackController.h"
#include "HapticInterface.h"
#include "DriverMessenger.h"
void DeviceContainer::AddDevice(nsvr_device_id id, PluginApis & apis, Parsing::BodyGraphDescriptor bodyGraphDescriptor)
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
			addDevice(desc, apis, std::move(bodyGraphDescriptor));
		}
	}

}


void DeviceContainer::addDevice(const DeviceDescriptor& desc, PluginApis& apis, Parsing::BodyGraphDescriptor bodyGraphDescriptor)
{
	
	auto playback = std::make_unique<HardwarePlaybackController>(apis.GetApi<playback_api>());

	auto bodygraph = std::make_shared<HardwareBodygraphCreator>(bodyGraphDescriptor, apis.GetApi<bodygraph_api>());

	auto nodes = std::make_unique<HardwareNodeEnumerator>(desc.id, apis.GetApi<device_api>());

	auto haptics = std::make_unique<HapticInterface>(apis.GetApi<buffered_api>(), apis.GetApi<waveform_api>());

	m_deviceLock.lock();

	m_devices.push_back(std::make_unique<Device2>(desc, std::move(nodes), bodygraph, std::move(playback), std::move(haptics)));
	m_simulations.push_back(std::make_unique<SimulatedDevice>(desc.id, apis, bodygraph));

	m_deviceLock.unlock();
	notify(m_deviceAddedSubs, m_devices.back().get());
}




void DeviceContainer::RemoveDevice(nsvr_device_id id)
{
	for (const auto& device : m_devices) {
		if (device->id() == id) {
			notify(m_deviceRemovedSubs, device.get());
		}
	}

	m_deviceLock.lock();

	auto rem = std::remove_if(m_devices.begin(), m_devices.end(), [id](const auto& device) { return device->id() == id; });
	m_devices.erase(rem, m_devices.end());

	auto rem2 = std::remove_if(m_simulations.begin(), m_simulations.end(), [id](const auto& device) { return device->id() == id; });
	m_simulations.erase(rem2, m_simulations.end());

	m_deviceLock.unlock();
}

void DeviceContainer::Each(std::function<void(Device2*)> forEach)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);
	for (auto& ptr : m_devices) {
		forEach(ptr.get());
	}
}

void DeviceContainer::EachSimulation(std::function<void(SimulatedDevice*)> action) {
	std::lock_guard<std::mutex> guard(m_deviceLock);
	for (auto& ptr : m_simulations) {
		action(ptr.get());
	}
}


DeviceContainer::DeviceContainer() 
{
	
}

Device2* DeviceContainer::Get(nsvr_device_id id)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);

	auto it = std::find_if(m_devices.begin(), m_devices.end(), [id](const auto& device) { return device->id() == id; });
	if (it != m_devices.end()) {
		return (*it).get();
	}
	else {
		return nullptr;
	}
}

void DeviceContainer::OnDeviceAdded(DeviceFn fn)
{
	m_deviceAddedSubs.push_back(fn);
}

void DeviceContainer::OnPreDeviceRemoved(DeviceFn fn)
{
	m_deviceRemovedSubs.push_back(fn);
}

void DeviceContainer::notify(const std::vector<DeviceFn>& subscribers, Device2 * device)
{
	for (const auto& fn : subscribers) {
		fn(device);
	}
}

