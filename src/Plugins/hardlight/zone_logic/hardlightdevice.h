#pragma once

#include <vector>
#include <memory>
#include "HardwareCommands.h"
#include "HardlightMk3ZoneDriver.h"

typedef struct NSVR_Core_t NSVR_Core;
class HardlightDevice {
public:
	HardlightDevice();
	using RegisterFunc = std::function<void(NSVR_Consumer_Handler_t, const char*, const char*, void*)>;
	void RegisterDrivers(const RegisterFunc& registerFunc);


	CommandBuffer GenerateHardwareCommands(float dt);

//	template<typename THapticType>
	//void callback(void* client_data, const char* region, const char* iface, const NSVR_GenericEvent* event);


	//virtual DisplayResults QueryDrivers() override;
private:

	std::vector<std::unique_ptr<Hardlight_Mk3_ZoneDriver>> m_drivers;


};
template<typename THaptic>
void zoneDriverCallback(void* client_data, const char* region, const char* iface, const NSVR_GenericEvent* event) {
	Hardlight_Mk3_ZoneDriver* driver = static_cast<Hardlight_Mk3_ZoneDriver*>(client_data);
	driver->consume(AS_TYPE(const THaptic, event));
}



