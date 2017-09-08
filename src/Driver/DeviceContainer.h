#pragma once
#include <memory>
#include "IHardwareDevice.h"
#include <unordered_map>

#include <mutex>
#include "DriverConfigParser.h"

struct DeviceDescriptor {
	std::string displayName;
	nsvr_device_id id;
	nsvr_device_concept concept;
};

// Should break this into two interfaces: adding/removing, and the data operations
class DeviceContainer {
public:
	using DeviceFn = std::function<void(Device*)>;
	void AddDevice(nsvr_device_id id, PluginApis& apis, PluginEventSource& ev, Parsing::BodyGraphDescriptor);
	void RemoveDevice(const std::string&);
	
	void RemoveDevice(uint64_t id);
	void Each(DeviceFn);

	DeviceContainer();


	void OnDeviceAdded(DeviceFn);
	void OnPreDeviceRemoved(DeviceFn);

private:
	void addDevice(const DeviceDescriptor&, PluginApis&, PluginEventSource&, Parsing::BodyGraphDescriptor);

	std::vector<std::unique_ptr<Device>> m_devices;

	std::vector<DeviceFn> m_deviceAddedSubs;
	std::vector<DeviceFn> m_deviceRemovedSubs;

	std::mutex m_deviceLock;
	void notify(const std::vector<DeviceFn>& devices, Device* device);
	
};


