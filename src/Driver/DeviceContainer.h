#pragma once
#include <memory>
#include "IHardwareDevice.h"
#include <unordered_map>


// Should break this into two interfaces: adding/removing, and the data operations
class DeviceContainer {
public:
	void AddDevice(const HardwareDescriptor&, PluginApis&, PluginEventHandler&);
	void RemoveDevice(const std::string&);
	
	
	void All(std::function<void(NodalDevice*)>);

	//temp function for testing
	TrackingNode* GetFirstTrackedDevice();



private:
	
	std::vector<std::unique_ptr<NodalDevice>> m_devices;

	
};


