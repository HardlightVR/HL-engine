#include "stdafx.h"
#include "DeviceContainer.h"
#include <experimental/vector>


void DeviceContainer::addDevice(const DeviceDescriptor& desc, PluginApis& apis, PluginEventSource& ev, Parsing::BodyGraphDescriptor bodyGraphDescriptor)
{
	m_deviceLock.lock();
	m_devices.push_back(std::make_unique<Device>(desc, apis, ev, std::move(bodyGraphDescriptor)));
	m_deviceLock.unlock();


	notify(m_deviceAddedSubs, m_devices.back().get());
}

void DeviceContainer::AddDevice(nsvr_device_id id, PluginApis & apis, PluginEventSource & ev, Parsing::BodyGraphDescriptor bodyGraphDescriptor)
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
			addDevice(desc, apis, ev, std::move(bodyGraphDescriptor));
		}
	}

}

void DeviceContainer::RemoveDevice(const std::string & name)
{
	for (const auto& device : m_devices) {
		if (device->name() == name) {
			notify(m_deviceRemovedSubs, device.get());
		}
	}

	m_deviceLock.lock();
	auto rem = std::remove_if(m_devices.begin(), m_devices.end(), [&](const auto& device) { return device->name() == name; });
	m_devices.erase(rem, m_devices.end());
	m_deviceLock.unlock();
}


void DeviceContainer::RemoveDevice(uint64_t id)
{
	for (const auto& device : m_devices) {
		if (device->id() == id) {
			notify(m_deviceRemovedSubs, device.get());
		}
	}

	m_deviceLock.lock();
	auto rem = std::remove_if(m_devices.begin(), m_devices.end(), [&](const auto& device) { return device->id() == id; });
	m_devices.erase(rem, m_devices.end());
	m_deviceLock.unlock();
}

void DeviceContainer::Each(std::function<void(Device*)> forEach)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);
	for (auto& ptr : m_devices) {
		forEach(ptr.get());
	}
}



DeviceContainer::DeviceContainer() 
{
	
}

void DeviceContainer::OnDeviceAdded(DeviceFn fn)
{
	m_deviceAddedSubs.push_back(fn);
}

void DeviceContainer::OnPreDeviceRemoved(DeviceFn fn)
{
	m_deviceRemovedSubs.push_back(fn);
}

void DeviceContainer::notify(const std::vector<DeviceFn>& subscribers, Device * device)
{
	for (const auto& fn : subscribers) {
		fn(device);
	}
}

