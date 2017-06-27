#pragma once

#include <vector>
#include <memory>
#include "HardwareCommands.h"
#include "HardlightMk3ZoneDriver.h"

typedef struct NSVR_Core_t NSVR_Core;
class HardlightDevice {
public:
	HardlightDevice();

	void RegisterDrivers(NSVR_Core* core);


	CommandBuffer GenerateHardwareCommands(float dt);




	//virtual DisplayResults QueryDrivers() override;

private:
	std::vector<std::unique_ptr<Hardlight_Mk3_ZoneDriver>> m_drivers;


};




