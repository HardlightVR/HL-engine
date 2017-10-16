#pragma once
#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include "Device.h"
#include "PluginAPI.h"
#include "PluginApis.h"
#include "DriverConfigParser.h"
#include <boost/signals2.hpp>
#include "PluginInstance.h"

class DriverMessenger;
class DeviceContainer {
public:
	using DeviceEvent = boost::signals2::signal<void(Device*)>;
	using DeviceFn = std::function<void(Device*)>;

	DeviceContainer();
	void AddDevice(nsvr_device_id id, PluginApis& apis, std::string originatingPlugin, PluginInstance::DeviceResourceBundle& resources);
	void RemoveDevice(DeviceId<local> id, std::string pluginName);
	void EachDevice(DeviceFn action);

	Device* Get(std::string pluginName, DeviceId<local> id);

	//these functions should be very short as a lock is held during them
	void OnDeviceAdded(DeviceEvent::slot_type slot);
	void OnDeviceRemoved(DeviceEvent::slot_type slot);

private:
	DeviceEvent m_onDeviceAdded;
	DeviceEvent m_onDeviceRemoved;

	std::vector<std::unique_ptr<Device>> m_devices;
	std::vector<DeviceFn> m_deviceAddedSubs;
	std::vector<DeviceFn> m_deviceRemovedSubs;

	std::mutex m_deviceLock;
	
};


