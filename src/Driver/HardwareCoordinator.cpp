#include "stdafx.h"
#include "HardwareCoordinator.h"
#include "SharedCommunication/SharedTypes.h"
#include "DriverMessenger.h"
#include <iostream>
#include "cevent_internal.h"
#include "nsvr_playback_handle.h"
#include "EventDispatcher.h"

HardwareCoordinator::HardwareCoordinator(DriverMessenger& messenger, EventDispatcher& dispatcher) : m_messenger(messenger)
{
	dispatcher.Subscribe(NullSpaceIPC::HighLevelEvent::kSimpleHaptic, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_activeEffects[event.parent_id()] = { event.parent_id() };
		const auto& simple_haptic = event.simple_haptic();
		this->Dispatch(simple_haptic, &m_activeEffects.at(event.parent_id()));
	});

	dispatcher.Subscribe(NullSpaceIPC::HighLevelEvent::kPlaybackEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		const auto& playback_event = event.playback_event();

		nsvr_playback_handle* existingPtr = &m_activeEffects.at(event.parent_id());

		this->Playback(playback_event.command(), existingPtr);
		
		if (playback_event.command() == 3) { //cancel
			//we now invalidate the playback_handle ptr by erasing from here
			//so the user better not be still using it
			m_activeEffects.erase(event.parent_id());
		}
		
	});
}

HardwareCoordinator::~HardwareCoordinator()
{
}

void HardwareCoordinator::Register(nsvr_request_type type, nsvr_request_api::nsvr_request_handler handler, unsigned int targetVersion, void * user_data)
{
	user_event_handler eventHandler;
	eventHandler.invoke = handler;
	eventHandler.user_data = user_data;
	eventHandler.target_version = targetVersion;

	m_handlers[type].push_back(eventHandler);

}



HardwareDataModel & HardwareCoordinator::Get(const std::string & name)
{
	if (m_hardware.find(name) == m_hardware.end()) {
		m_hardware.insert(std::make_pair(name, HardwareDataModel(*this)));

		//m_hardware.at(name).OnTrackingUpdate([&](auto region, auto quat) { updateTrackingForMessenger(region, quat); });
		//m_hardware.at(name).OnDeviceConnect([&]() {
		//	SuitsConnectionInfo info = { };
		//	info.SuitsFound[0] = true;
		//	info.Suits[0].Id = 1;
		//	info.Suits[0].Status = NullSpace::SharedMemory::Connected;
		//	m_messenger.WriteSuits(info);
		//});

		//m_hardware.at(name).OnDeviceConnect([&]() {
		//	std::cout << name << " CONNECTED\n";
		//});

		//m_hardware.at(name).OnDeviceDisconnect([&]() {
		//	std::cout << name << " DISCONNECTED\n";
		//});

		//m_hardware.at(name).OnDeviceDisconnect([&]() {
		//	SuitsConnectionInfo info = { };
		//	info.SuitsFound[0] = true;
		//	info.Suits[0].Id = 1;
		//	info.Suits[0].Status = NullSpace::SharedMemory::Disconnected;
		//	m_messenger.WriteSuits(info);
		//});
	}
	return m_hardware.at(name);
	
}

void HardwareCoordinator::dispatch_event(nsvr::cevents::request_base& event)
{

	for (auto& handler : m_handlers[event.getType()]) {
		nsvr_request* r = reinterpret_cast<nsvr_request*>(&event);
		handler.invoke(r, handler.user_data);
	}
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

	m_messenger.WriteTracking(t);
}


void HardwareCoordinator::Dispatch(const NullSpaceIPC::SimpleHaptic& event, nsvr_playback_handle* handle)
{
	
	for (const auto& regionStr : event.regions()) {
		auto lasting = nsvr::cevents::LastingHaptic(
			event.effect(),
			event.strength(),
			event.duration(),
			regionStr.c_str()
		);
		lasting.handle = handle;

		dispatch_event(lasting);
	}
	
}

void HardwareCoordinator::Playback(uint32_t command, nsvr_playback_handle* existingHandle)
{
	for (auto& device : m_hardware) {
		device.second.Playback(command, existingHandle);
	}
}

