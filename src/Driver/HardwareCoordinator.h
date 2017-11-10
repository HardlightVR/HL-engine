#pragma once
#include <boost/signals2/signal.hpp>
#include <boost/asio/io_service.hpp>
#include "PluginAPI.h"
#include "IdentificationService.h"
#include "ScheduledEvent.h"
#include "protobuff_defs/HighLevelEvent.pb.h"

class EventDispatcher;
class DeviceContainer;
class DriverMessenger;


class HardwareCoordinator
{
public:
	using TrackingSignal = boost::signals2::signal<void(nsvr_node_id, nsvr_quaternion*)>;
	HardwareCoordinator(boost::asio::io_service&, DriverMessenger& messenger, DeviceContainer& devices);
	void SetupSubscriptions(EventDispatcher& dispatcher);
private:
	IdentificationService m_idService;

	DriverMessenger& m_messenger;
	DeviceContainer& m_devices;

	void writeTracking(nsvr_node_id region, nsvr_quaternion* quat);
	ScheduledEvent m_writeBodyRepresentation;

	void genericDispatch(uint64_t id, const NullSpaceIPC::LocationalEvent& event);
	void writeBodyRepresentation();



	std::unordered_map<NullSpaceIPC::LocationalEvent::EventsCase,
		std::function<void(uint64_t, const NullSpaceIPC::LocationalEvent&)>> m_dispatchTable;
};



