#pragma once
#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include "Device.h"
#include "SimulatedDevice.h"
#include "PluginAPI.h"
#include "PluginApis.h"
#include "DriverConfigParser.h"

class DriverMessenger;
class DeviceContainer {
public:
	using DeviceFn = std::function<void(Device*)>;
	void AddDevice(nsvr_device_id id, PluginApis& apis, Parsing::BodyGraphDescriptor, std::string originatingPlugin);
	
	void RemoveDevice(nsvr_device_id id);
	void Each(DeviceFn);
	void EachSimulation(std::function<void(SimulatedDevice*)> action);
	DeviceContainer();

	Device* Get(nsvr_device_id id);

	void OnDeviceAdded(DeviceFn);
	void OnPreDeviceRemoved(DeviceFn);

private:
	void addDevice(const DeviceDescriptor&, PluginApis&,  Parsing::BodyGraphDescriptor, std::string originatingPlugin);

	std::vector<std::unique_ptr<Device>> m_devices;
	std::vector<std::unique_ptr<SimulatedDevice>> m_simulations;
	std::vector<DeviceFn> m_deviceAddedSubs;
	std::vector<DeviceFn> m_deviceRemovedSubs;

	std::mutex m_deviceLock;
	void notify(const std::vector<DeviceFn>& devices, Device* device);
	
};


