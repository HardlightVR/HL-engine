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
#include <boost/signals2.hpp>

class IdentificationService;
class DriverMessenger;
class DeviceContainer {
public:
	using DeviceEvent = boost::signals2::signal<void(Device*)>;
	using DeviceFn = std::function<void(Device*)>;
	using SimFn = std::function<void(SimulatedDevice*)>;

	DeviceContainer(IdentificationService& idService);
	void AddDevice(nsvr_device_id id, PluginApis& apis, Parsing::BodyGraphDescriptor, std::string originatingPlugin);
	
	void RemoveDevice(nsvr_device_id id);
	void EachDevice(DeviceFn action);
	void EachSimulation(SimFn action);

	Device* Get(nsvr_device_id id);

	//these functions should be very short as a lock is held during them
	void OnDeviceAdded(DeviceEvent::slot_type slot);
	void OnDeviceRemoved(DeviceEvent::slot_type slot);

private:
	void addDevice(const DeviceDescriptor&, PluginApis&,  Parsing::BodyGraphDescriptor, std::string originatingPlugin);
	IdentificationService& m_idService;
	DeviceEvent m_onDeviceAdded;
	DeviceEvent m_onDeviceRemoved;

	std::vector<std::unique_ptr<Device>> m_devices;
	std::vector<std::unique_ptr<SimulatedDevice>> m_simulations;
	std::vector<DeviceFn> m_deviceAddedSubs;
	std::vector<DeviceFn> m_deviceRemovedSubs;

	std::mutex m_deviceLock;
	
};


