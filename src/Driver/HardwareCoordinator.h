#pragma once

class EventDispatcher;
class DeviceContainer;
class DriverMessenger;
struct nsvr_quaternion;
class HardwareCoordinator
{
public:
	HardwareCoordinator(DriverMessenger& messenger, DeviceContainer& devices, EventDispatcher& dispatcher);


	~HardwareCoordinator() = default;


private:
	void setupSubscriptions(EventDispatcher& dispatcher);
	DriverMessenger& m_messenger;
	DeviceContainer& m_devices;

	void writeTracking(const char* region, nsvr_quaternion* quat);
};



