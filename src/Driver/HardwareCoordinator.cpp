#include "stdafx.h"
#include "HardwareCoordinator.h"
#include "SharedCommunication/SharedTypes.h"
#include "DriverMessenger.h"
#include <iostream>

HardwareCoordinator::HardwareCoordinator(DriverMessenger& messenger): m_messenger(messenger)
{
	
}

HardwareCoordinator::~HardwareCoordinator()
{
}

void HardwareCoordinator::Register(nsvr_cevent_type type, nsvr_cevent_handler handler, unsigned int targetVersion, void * user_data)
{
	user_event_handler eventHandler;
	eventHandler.handler = handler;
	eventHandler.user_data = user_data;
	eventHandler.target_version = targetVersion;

	m_handlers[type].push_back(eventHandler);

}



brief_haptic getBriefHapticVersion(const unsigned int version, float strength, int effect, const char* region) {
	if (version == 1) {
		return nsvr_cevent_brief_haptic_v1{};
	}
	else if (version == 2) {
		return nsvr_cevent_brief_haptic_v2{};
	}
	else {
		return boost::blank{};
	}
	
}

//latest interface
void HardwareCoordinator::dispatchBriefHaptic(float strength, int effect, const char* region) {
	for (auto& handler : m_handlers[nsvr_cevent_type_brief_haptic]) {
		brief_haptic thing = getBriefHapticVersion(handler.target_version, strength, effect, region);

		void* ptr = boost::apply_visitor([](auto& x) -> void* { return std::addressof(x); }, thing);
		handler.handler(ptr, nsvr_cevent_type_brief_haptic, handler.user_data);
	}
}

HardwareDataModel & HardwareCoordinator::Get(const std::string & name)
{
	/*if (m_hardware.find(name) == m_hardware.end()) {
		m_hardware[name] = HardwareDataModel(*this);
		m_hardware.at(name).OnTrackingUpdate([&](auto region, auto quat) { updateTrackingForMessenger(region, quat); });
		m_hardware.at(name).OnDeviceConnect([&]() {
			SuitsConnectionInfo info = { };
			info.SuitsFound[0] = true;
			info.Suits[0].Id = 1;
			info.Suits[0].Status = NullSpace::SharedMemory::Connected;
			m_messenger.WriteSuits(info);
		});

		m_hardware.at(name).OnDeviceConnect([&]() {
			std::cout << name << " CONNECTED\n";
		});

		m_hardware.at(name).OnDeviceDisconnect([&]() {
			std::cout << name << " DISCONNECTED\n";
		});

		m_hardware.at(name).OnDeviceDisconnect([&]() {
			SuitsConnectionInfo info = { };
			info.SuitsFound[0] = true;
			info.Suits[0].Id = 1;
			info.Suits[0].Status = NullSpace::SharedMemory::Disconnected;
			m_messenger.WriteSuits(info);
		});
	}
	return m_hardware.at(name);*/
	return HardwareDataModel();
}

void HardwareCoordinator::updateTrackingForMessenger(const std::string & region, NSVR_Core_Quaternion quat)
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

HardwareCoordinator::BriefHapticArgs::BriefHapticArgs(uint32_t effect, float strnegth, const char * region)
	: effect(effect),
	strength(strnegth),
	region(region)

{
}
