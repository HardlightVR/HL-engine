#pragma once

class EventDispatcher;
class DeviceContainer;
class DriverMessenger;
struct nsvr_quaternion;

#include "ScheduledEvent.h"

#include <boost/signals2/signal.hpp>
#include "HumanBodyNodes.h"
class HardwareCoordinator
{
public:
	HardwareCoordinator(boost::asio::io_service&, DriverMessenger& messenger, DeviceContainer& devices);


	~HardwareCoordinator() = default;
	void SetupSubscriptions(EventDispatcher& dispatcher);
	void Cleanup();
	void Hook_TrackingSlot(boost::signals2::signal<void(uint64_t, nsvr_quaternion*)>& hook);
private:
	DriverMessenger& m_messenger;
	DeviceContainer& m_devices;

	HumanBodyNodes m_bodyRepresentation;

	void hook_writeTracking(uint64_t region, nsvr_quaternion* quat);

	ScheduledEvent m_writeBodyRepresentation;

	void writeBodyRepresentation();
};



