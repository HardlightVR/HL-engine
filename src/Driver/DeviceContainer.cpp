#include "stdafx.h"
#include "DeviceContainer.h"

void DeviceContainer::AddDevice(const std::string &, std::unique_ptr<Device>)
{
}

void DeviceContainer::RemoveDevice(const std::string &)
{
}

void DeviceContainer::ForEachSuit(std::function<void(SuitDevice*)> fn)
{
	for (auto& device : m_devices) {
		if (SuitDevice* d = dynamic_cast<SuitDevice*>(device.second.get())) {
			fn(d);
		}
	}
}

void DeviceContainer::ForEachHapticDeviceInRegion(const std::vector<std::string>& regions, std::function<void(Device*, const char*)> fn)
{
	for (auto& device : m_devices) {
		for (const auto& region : regions) {
			if (device.second->supportsRegion(region)) {
				fn(device.second.get(), region.c_str());
			}
		}
	}
}

void DeviceContainer::ForEachDevice(std::function<void(Device*)> fn)
{
	for (auto& device : m_devices) {
		fn(device.second.get());
	}
}
