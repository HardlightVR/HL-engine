#pragma once

#include <vector>
#include <memory>
#include "HardwareCommands.h"
#include "HardlightMk3ZoneDriver.h"

typedef struct NSVR_Core_t NSVR_Core;
class HardlightDevice {
public:
	HardlightDevice();
//	using RegisterFunc = std::function<void(nsvr_event_handler, const char*, const char*, void*)>;
	void RegisterDrivers(nsvr_core_ctx* ctx);


	CommandBuffer GenerateHardwareCommands(float dt);

//	template<typename THapticType>
	//void callback(void* client_data, const char* region, const char* iface, const NSVR_GenericEvent* event);


	//virtual DisplayResults QueryDrivers() override;
	void handle(nsvr_cevent* event);
private:

	std::vector<std::unique_ptr<Hardlight_Mk3_ZoneDriver>> m_drivers;


};
template<typename THaptic>
void makeCallback(void* client_data, const char* region, const char* iface, const NSVR_GenericEvent* event) {
	Hardlight_Mk3_ZoneDriver* driver = static_cast<Hardlight_Mk3_ZoneDriver*>(client_data);
	driver->consume(AS_TYPE(const THaptic, event));
}



