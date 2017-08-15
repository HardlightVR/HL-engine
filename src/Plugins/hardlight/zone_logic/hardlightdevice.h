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
	void Configure(nsvr_core* ctx);


	CommandBuffer GenerateHardwareCommands(float dt);

	void handle(nsvr_request* event);

	void Pause(ParentId  handle);
	void Cancel(ParentId  handle);
	void Unpause(ParentId  handle);
	int Query(nsvr_region node, nsvr_sampling_sample* outState);

	void EnumerateDeviceIds(nsvr_device_ids* ids);
	void GetDeviceInfo(uint64_t id, nsvr_device_basic_info* info);
	
private:

	std::unordered_map<nsvr_region, std::unique_ptr<Hardlight_Mk3_ZoneDriver>> m_drivers;
	template<typename T>
	void execute_region_specific(void* regioned_event);
	void executeLasting(nsvr_request * event);
};

template<typename T>
inline void HardlightDevice::execute_region_specific(void * regioned_event)
{
	T* ev = static_cast<T*>(regioned_event);
	nsvr_region region = ev->region;
	if (m_drivers.find(region) != m_drivers.end()) {
		m_drivers.at(region)->consume(ev);
	}
}
