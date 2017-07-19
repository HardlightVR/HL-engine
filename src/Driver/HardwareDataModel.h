#pragma once


#include "PluginAPI.h"
#include "LowLevelInterface.h"

namespace nsvr {
	namespace pevents {
		class device_event;
	}
}
class HardwareCoordinator;
class HardwareDataModel {
public:
	HardwareDataModel(HardwareCoordinator& parentCoordinator);
	HardwareDataModel();


	void Raise(const nsvr::pevents::device_event& event);

	HardwareCoordinator& GetParentCoordinator();

	LowLevelInterface& LowLevel();

private:
	
	std::unordered_map<std::string, nsvr_quaternion> m_trackingData;
	LowLevelInterface m_lowlevel;
	HardwareCoordinator& m_parent;
	bool m_connected;
	
};