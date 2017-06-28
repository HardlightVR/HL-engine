#pragma once


#include "HardwareDataModel.h"

class DriverMessenger;
class HardwareCoordinator
{
public:
	HardwareCoordinator(DriverMessenger& messenger);
	~HardwareCoordinator();


	HardwareDataModel& Get(const std::string& name);
private:
	DriverMessenger& m_messenger;
	std::unordered_map<std::string, HardwareDataModel> m_hardware;

	void updateTrackingForMessenger(const std::string& region, NSVR_Core_Quaternion quat);
};

