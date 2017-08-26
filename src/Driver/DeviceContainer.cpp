#include "stdafx.h"
#include "DeviceContainer.h"
#include <experimental/vector>


void DeviceContainer::AddDevice(const HardwareDescriptor& desc, PluginApis& apis, PluginEventSource& ev)
{
	m_deviceLock.lock();
	m_devices.push_back(std::make_unique<NodalDevice>(desc, apis, ev));
	m_deviceLock.unlock();

	notify(m_deviceAddedSubs, m_devices.back().get());
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

void DeviceContainer::Each(std::function<void(NodalDevice*)> forEach)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);
	for (auto& ptr : m_devices) {
		forEach(ptr.get());
	}
}



void DeviceContainer::OnDeviceAdded(DeviceFn fn)
{
	m_deviceAddedSubs.push_back(fn);
}

void DeviceContainer::OnDeviceRemoved(DeviceFn fn)
{
	m_deviceRemovedSubs.push_back(fn);
}

void DeviceContainer::notify(const std::vector<DeviceFn>& subscribers, NodalDevice * device)
{
	for (const auto& fn : subscribers) {
		fn(device);
	}
}

