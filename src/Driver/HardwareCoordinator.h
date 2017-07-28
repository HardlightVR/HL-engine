#pragma once
#include <type_traits>

#include "cevent_internal.h"

#include "protobuff_defs/HighLevelEvent.pb.h"
#include <unordered_map>
#include <boost/asio/io_service.hpp>
#include "ScheduledEvent.h"
#include "IHardwareDevice.h"

class DriverMessenger;
class EventDispatcher;
class DeviceContainer;
class HardwareCoordinator
{
public:
	HardwareCoordinator(DeviceContainer& devices, EventDispatcher& dispatcher);
	~HardwareCoordinator();

	//void Register(nsvr_request_type type, nsvr_plugin_request_api::nsvr_request_handler handler, unsigned int, void* user_data);
	//HardwareDataModel& Get(const std::string& name);

	void dispatch_event(nsvr::cevents::request_base& event);



	//void HardwareCoordinator::Playback(uint32_t command,uint64_t existingHandle);



private:
	
	struct user_event_handler {
		nsvr_plugin_request_api::nsvr_request_handler invoke;
		void* user_data;
		unsigned int target_version;
	};
	std::unordered_map<nsvr_request_type, std::vector<user_event_handler>> m_handlers;
	void updateTrackingForMessenger(const std::string& region, nsvr_quaternion quat);

	
	DeviceContainer& m_devices;
//	ScheduledEvent m_pollDataModels;
};



