#pragma once

class EventDispatcher;
class DeviceContainer;
class HardwareCoordinator
{
public:
	HardwareCoordinator(DeviceContainer& devices, EventDispatcher& dispatcher);


	~HardwareCoordinator() = default;


private:
	void setupSubscriptions(EventDispatcher& dispatcher);

	DeviceContainer& m_devices;
};



