#pragma once

#include <vector>
#include <memory>
#include "HardwareCommands.h"
#include "HardlightMk3ZoneDriver.h"
#include <unordered_map>
typedef struct NSVR_Core_t NSVR_Core;
class HardlightDevice {
public:
	HardlightDevice();
	void RegisterDrivers(nsvr_core* ctx);


	CommandBuffer GenerateHardwareCommands(float dt);

	void handle(nsvr_request* event);

	void Pause(ParentId  handle);
	void Cancel(ParentId  handle);
	void Unpause(ParentId  handle);
private:

	std::unordered_map<std::string, std::unique_ptr<Hardlight_Mk3_ZoneDriver>> m_drivers;
	template<typename T>
	void execute_region_specific(void* regioned_event);
	void executeLasting(nsvr_request * event);
};

template<typename T>
inline void HardlightDevice::execute_region_specific(void * regioned_event)
{
	T* ev = static_cast<T*>(regioned_event);
	const char* region = ev->region;
	if (m_drivers.find(region) != m_drivers.end()) {
		m_drivers.at(region)->consume(ev);
	}
}
