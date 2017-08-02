#pragma once
#include <memory>
#include "IHardwareDevice.h"
#include <unordered_map>


// Should break this into two interfaces: adding/removing, and the data operations
class DeviceContainer {
public:
	using DeviceFn = std::function<void(NodalDevice*)>;
	void AddDevice(const HardwareDescriptor&, PluginApis&, PluginEventHandler&);
	void RemoveDevice(const std::string&);
	
	
	void Each(DeviceFn);



	void OnDeviceAdded(DeviceFn);
	void OnDeviceRemoved(DeviceFn);

private:


	std::vector<std::unique_ptr<NodalDevice>> m_devices;

	std::vector<DeviceFn> m_deviceAddedSubs;
	std::vector<DeviceFn> m_deviceRemovedSubs;

	void notify(const std::vector<DeviceFn>& devices, NodalDevice* device);
	
};


