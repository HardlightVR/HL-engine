#pragma once

class EventDispatcher;
class DeviceContainer;
class DriverMessenger;
struct nsvr_quaternion;

#include "ScheduledEvent.h"
#include "PluginAPI.h"
#include <boost/signals2/signal.hpp>
class HardwareCoordinator
{
public:
	using TrackingSignal = boost::signals2::signal<void(nsvr_node_id, nsvr_quaternion*)>;
	HardwareCoordinator(boost::asio::io_service&, DriverMessenger& messenger, DeviceContainer& devices);


	~HardwareCoordinator() = default;
	void SetupSubscriptions(EventDispatcher& dispatcher);
	void Cleanup();
	void Hook_TrackingSlot(TrackingSignal& hook);
private:
	DriverMessenger& m_messenger;
	DeviceContainer& m_devices;


	void writeTracking(nsvr_node_id region, nsvr_quaternion* quat);

	ScheduledEvent m_writeBodyRepresentation;

	void writeBodyRepresentation();
};



