#include "stdafx.h"
#include "DeviceContainer.h"

#include <experimental/vector>
#include "HardwareBodygraphCreator.h"
#include "HardwareNodeEnumerator.h"
#include "HardwarePlaybackController.h"
#include "HapticInterface.h"
#include "DriverMessenger.h"

//todo: we need a translation table from device -> user facing device

DeviceContainer::DeviceContainer(IdentificationService & idService)
	: m_idService(idService)
	, m_onDeviceAdded()
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
			addDevice(desc, apis, std::move(bodyGraphDescriptor), originatingPlugin, m_idService);
		}
	}

}


void DeviceContainer::addDevice(const DeviceDescriptor& desc, PluginApis& apis, Parsing::BodyGraphDescriptor bodyGraphDescriptor, std::string originatingPlugin, IdentificationService& idService)
{
	
	auto playback = std::make_unique<HardwarePlaybackController>(apis.GetApi<playback_api>());

	auto bodygraph = std::make_shared<HardwareBodygraphCreator>(bodyGraphDescriptor, apis.GetApi<bodygraph_api>());

	auto nodes = std::make_unique<HardwareNodeEnumerator>(desc.id, apis.GetApi<device_api>());
	

	auto haptics = std::make_unique<HapticInterface>(apis.GetApi<buffered_api>(), apis.GetApi<waveform_api>());

	m_deviceLock.lock();

	m_devices.push_back(std::make_unique<Device>(originatingPlugin, desc, bodygraph, std::move(nodes), std::move(playback), std::move(haptics), idService));
	m_simulations.push_back(std::make_unique<SimulatedDevice>(desc.id, apis, bodygraph));

	m_deviceLock.unlock();

	m_onDeviceAdded(m_devices.back().get());
}




void DeviceContainer::RemoveDevice(nsvr_device_id id)
{
	m_deviceLock.lock();

	auto it = std::find_if(m_devices.begin(), m_devices.end(), [id](const auto& device) { return device->id() == id; });
	if (it != m_devices.end()) {
		m_onDeviceRemoved(it->get());
		m_devices.erase(it);
		auto sim = std::find_if(m_simulations.begin(), m_simulations.end(), [id](const auto& device) { return device->id() == id; });
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


Device* DeviceContainer::Get(nsvr_device_id id)
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

void DeviceContainer::OnDeviceAdded(DeviceEvent::slot_type slot)
{
	m_onDeviceAdded.connect(slot);
}

void DeviceContainer::OnDeviceRemoved(DeviceEvent::slot_type slot)
{
	m_onDeviceRemoved.connect(slot);
}


