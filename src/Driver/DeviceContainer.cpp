#include "stdafx.h"
#include "DeviceContainer.h"
#include "Device.h"
#include "PluginApis.h"
#include "DeviceBuilder.h"

DeviceContainer::~DeviceContainer()
{
	{
		std::cout << "CONTAINER DESTRUCTOR\n";
	}
}

DeviceContainer::DeviceContainer()

	: m_onDeviceAdded()
	, m_onDeviceRemoved()
	, m_devices()
	, m_deviceAddedSubs()
	, m_deviceRemovedSubs()
	, m_deviceLock()
{
}

void DeviceContainer::AddDevice(nsvr_device_id id, PluginApis & apis, std::string originatingPlugin, PluginInstance::DeviceResources* resources)
{

	DeviceDescriptor descriptor;
	if (resources->deviceDescriptor) {
		descriptor = *resources->deviceDescriptor;

	}
	else {
		nsvr_device_info info = { { 0 }, nsvr_device_concept_unknown };
		apis.GetApi<device_api>()->submit_getdeviceinfo(id, &info);

		DeviceDescriptor desc;
		desc.displayName = std::string(info.name);
		desc.id = id;
		desc.concept = info.concept;
		descriptor = desc;
	}

	auto device = DeviceBuilder(&apis, resources, id)
		.WithDescriptor(descriptor)
		.WithOriginatingPlugin(originatingPlugin)
		.Build();


			m_deviceLock.lock();
			m_devices.push_back(std::move(device));
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



Device* DeviceContainer::Get(LocalDevice deviceId)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);

	auto it = std::find_if(m_devices.begin(), m_devices.end(), [&](const auto& device) {
		return device->id() == deviceId.id && device->parentPlugin() == deviceId.plugin; 
	});

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


