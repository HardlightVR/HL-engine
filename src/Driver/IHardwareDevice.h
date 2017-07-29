#pragma once

#include<memory>
#include "DriverConfigParser.h"
#include "cevent_internal.h"
#include <vector>
class PluginCapabilities;
class PluginEventHandler;

class NodalDevice {
public:
//	std::vector<std::unique_ptr<Node>> nodes;
	void handleDeviceRequest();
	void handleNodeRequest();
	
};

class Node {
public:
	virtual ~Node() {}
};

class LedDevice : public Node {
public:
	LedDevice(PluginCapabilities&, PluginEventHandler&);

};
class HapticDevice : public Node {
public:
	HapticDevice(PluginCapabilities&, PluginEventHandler&);

};
class TrackedDevice : public Node {
public:
	TrackedDevice(PluginCapabilities&, PluginEventHandler&);

};




namespace device_factories {
	std::vector<std::unique_ptr<NodalDevice>> createDevices(
		const HardwareDescriptor& description,
		PluginCapabilities& capabilities,
		PluginEventHandler& eventDispatcher);

	//std::unique_ptr<NodalDevice> createDevice(const HardwareDescriptor& description, PluginCapabilities&, PluginEventHandler&);

}



