#pragma once
#include <memory>
#include "IHardwareDevice.h"
#include <unordered_map>


// Should break this into two interfaces: adding/removing, and the data operations
class DeviceContainer {
public:
	void AddDevice(const std::string&, std::unique_ptr<Device>);
	void RemoveDevice(const std::string&);
	void ForEachSuit(std::function<void(SuitDevice*)>);
	void ForEachHapticDeviceInRegion(const std::vector<std::string>& regions, std::function<void(Device*, const char*)>);
	void ForEachDevice(std::function<void(Device*)>);


private:
	
	std::unordered_map<std::string, std::unique_ptr<Device>> m_devices;

	
};
