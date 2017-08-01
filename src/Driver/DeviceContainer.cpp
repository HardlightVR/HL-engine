#include "stdafx.h"
#include "DeviceContainer.h"
#include <experimental/vector>


void DeviceContainer::AddDevice(const HardwareDescriptor& desc, PluginApis& apis, PluginEventHandler& ev)
{
	m_devices.push_back(std::make_unique<NodalDevice>(desc, apis, ev));
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

TrackingNode* DeviceContainer::GetFirstTrackedDevice()
{
	for (auto& ptr : m_devices) {
		//ptr->
	}
	return nullptr;
}

