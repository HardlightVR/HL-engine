#pragma once

#include<memory>
#include "DriverConfigParser.h"
#include "cevent_internal.h"

class PluginCapabilities;
class PluginEventHandler;

class Device {
public:
	virtual ~Device() {}
	virtual bool supportsRegion(const std::string& region) const = 0;
	virtual void doRequest(const nsvr::cevents::request_base&) = 0;
	
	virtual void controlPlayback(uint64_t id, uint32_t command) = 0;
};



//suit, gun, etc.
class SuitDevice : public Device{
public:
	SuitDevice(const HardwareDescriptor&, PluginCapabilities&, PluginEventHandler&);
	bool supportsRegion(const std::string& region) const override;
	void doRequest(const nsvr::cevents::request_base&) override;
	void controlPlayback(uint64_t id, uint32_t command) override;
private:
	PluginCapabilities& m_associatedPlugin;
	HardwareDescriptor m_descriptor;
};




namespace device_factories {
	std::unique_ptr<Device> createDevice(const HardwareDescriptor& description, PluginCapabilities&, PluginEventHandler&);

}



