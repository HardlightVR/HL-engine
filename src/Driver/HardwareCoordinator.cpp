#include "stdafx.h"
#include "HardwareCoordinator.h"
#include "SharedCommunication/SharedTypes.h"
#include "DriverMessenger.h"
#include <iostream>
#include "cevent_internal.h"
#include "nsvr_playback_handle.h"
#include "EventDispatcher.h"
#include "DeviceContainer.h"


struct matches_region {
	bool operator()(Node* node) {
		if (true /*node->region == chest_left*/) {
			result = "chest_left";
		}
		return true;
	}
	std::string result;
};
HardwareCoordinator::HardwareCoordinator(DeviceContainer& devices, EventDispatcher& dispatcher):
	 m_devices(devices)
{
	dispatcher.Subscribe(NullSpaceIPC::HighLevelEvent::kSimpleHaptic, [&](const NullSpaceIPC::HighLevelEvent& event) {
		const auto& simple_haptic = event.simple_haptic();

		std::vector<std::string> regions;
		for (const auto& r : simple_haptic.regions()) { regions.push_back(r); }

	

		m_devices.ForEachNode(matches_region(), [](Node* node, std::string result) {

		});
	/*	m_devices.ForEachHapticDeviceInRegion(regions, [&](NodalDevice* device, const char* region) {
			auto lasting = nsvr::cevents::LastingHaptic(
				simple_haptic.effect(),
				simple_haptic.strength(),
				simple_haptic.duration(),
				region
				);
			lasting.handle = event.parent_id();
			device->doRequest(lasting);
		});*/

	});

	dispatcher.Subscribe(NullSpaceIPC::HighLevelEvent::kPlaybackEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		const auto& playback_event = event.playback_event();

		/*m_devices.ForEachDevice([&](NodalDevice* device) {
			device->controlPlayback(event.parent_id(), playback_event.command());
		});*/
		
	});

	
}

HardwareCoordinator::~HardwareCoordinator()
{
}

//void HardwareCoordinator::Register(nsvr_request_type type, nsvr_request_api::nsvr_request_handler handler, unsigned int targetVersion, void * user_data)
//{
//	/*user_event_handler eventHandler;
//	eventHandler.invoke = handler;
//	eventHandler.user_data = user_data;
//	eventHandler.target_version = targetVersion;
//
//	m_handlers[type].push_back(eventHandler);*/
//
//}



void HardwareCoordinator::dispatch_event(nsvr::cevents::request_base& event)
{

	
}



void HardwareCoordinator::updateTrackingForMessenger(const std::string & region, nsvr_quaternion quat)
{
//	std::cout << "Got tracking data for region " << region << '\n';
	NullSpace::SharedMemory::TrackingUpdate t = {};

	//std::cout << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << '\n';
	t.chest.w = quat.w;
	t.chest.x = quat.x;
	t.chest.y = quat.y;
	t.chest.z = quat.z;

//	m_messenger.WriteTracking(t);
}


//void HardwareCoordinator::Playback(uint32_t command, uint64_t existingHandle)
//{
//	//for (auto& device : m_hardware) {
//	//	device.second.Playback(command, existingHandle);
//	//}
//}

