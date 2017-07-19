#include "stdafx.h"
#include "HardwareCoordinator.h"
#include "SharedCommunication/SharedTypes.h"
#include "DriverMessenger.h"
#include <iostream>
#include "cevent_internal.h"
HardwareCoordinator::HardwareCoordinator(DriverMessenger& messenger): m_messenger(messenger)
{
	
}

HardwareCoordinator::~HardwareCoordinator()
{
}

void HardwareCoordinator::Register(nsvr_request_type type, nsvr_request_handler handler, unsigned int targetVersion, void * user_data)
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


