#include "stdafx.h"
#include "DeviceContainer.h"
#include <experimental/vector>
void DeviceContainer::AddDevice(std::unique_ptr<NodalDevice> device)
{
	m_devices.push_back(std::move(device));
}

void DeviceContainer::RemoveDevice(const std::string & name)
{
	std::experimental::erase_if(m_devices, [&](const auto& device) {return device->name() == name; });
}

void DeviceContainer::All(std::function<void(NodalDevice*)> forEach)
{
	for (auto& ptr : m_devices) {
		forEach(ptr.get());
	}
}

