#pragma once
#include <memory>
#include "IHardwareDevice.h"
#include <unordered_map>

#include <mutex>
// Should break this into two interfaces: adding/removing, and the data operations
class DeviceContainer {
public:
	using DeviceFn = std::function<void(NodalDevice*)>;
	void AddDevice(const HardwareDescriptor&, PluginApis&, PluginEventSource&);
	void RemoveDevice(const std::string&);
	
	
	void Each(DeviceFn);



	void OnDeviceAdded(DeviceFn);
	void OnDeviceRemoved(DeviceFn);

private:


	std::vector<std::unique_ptr<NodalDevice>> m_devices;

	std::vector<DeviceFn> m_deviceAddedSubs;
	std::vector<DeviceFn> m_deviceRemovedSubs;

	std::mutex m_deviceLock;
	void notify(const std::vector<DeviceFn>& devices, NodalDevice* device);
	
};


