#include "stdafx.h"
#include "DeviceContainer.h"
#include <experimental/vector>


void DeviceContainer::AddDevice(const HardwareDescriptor& desc, PluginApis& apis, PluginEventHandler& ev)
{
	m_devices.push_back(std::make_unique<NodalDevice>(desc, apis, ev));
	notify(m_deviceAddedSubs, m_devices.back().get());
}

void DeviceContainer::RemoveDevice(const std::string & name)
{
	for (const auto& device : m_devices) {
		if (device->name() == name) {
			notify(m_deviceRemovedSubs, device.get());
		}
	}

	auto rem = std::remove_if(m_devices.begin(), m_devices.end(), [&](const auto& device) { return device->name() == name; });
	m_devices.erase(rem, m_devices.end());
}

void DeviceContainer::All(std::function<void(NodalDevice*)> forEach)
{
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

