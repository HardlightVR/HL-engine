#pragma once

class EventDispatcher;
class DeviceContainer;
class DriverMessenger;
struct nsvr_quaternion;

#include <boost/signals2/signal.hpp>
class HardwareCoordinator
{
public:
	HardwareCoordinator(DriverMessenger& messenger, DeviceContainer& devices, EventDispatcher& dispatcher);


	~HardwareCoordinator() = default;

	void RegisterTrackingSource(boost::signals2::signal<void(const char*, nsvr_quaternion*)>& hook);
	void UnregisterTrackingSource(boost::signals2::signal<void(const char*, nsvr_quaternion*)>& hook);
private:
	void setupSubscriptions(EventDispatcher& dispatcher);
	DriverMessenger& m_messenger;
	DeviceContainer& m_devices;

	void writeTracking(const char* region, nsvr_quaternion* quat);
};



