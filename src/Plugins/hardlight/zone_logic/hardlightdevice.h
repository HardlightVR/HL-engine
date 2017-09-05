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

	void handle(uint64_t request_id, uint64_t device_id, nsvr_waveform* wave);
	void Pause(ParentId  handle);
	void Cancel(ParentId  handle);
	void Unpause(ParentId  handle);

	void EnumerateNodesForDevice(uint32_t device_id, nsvr_node_ids* ids);
	void EnumerateDevices(nsvr_device_ids* ids);
	void GetDeviceInfo(uint32_t id, nsvr_device_info* info);
	void GetNodeInfo(uint64_t id, nsvr_node_info* info);
	void RaiseDeviceConnectionEvent(nsvr_core* core);
	void RaiseDeviceDisconnectionEvent(nsvr_core* core);
	void SetupDeviceAssociations(nsvr_bodygraph* g);
private:

	std::unordered_map<Location, std::unique_ptr<Hardlight_Mk3_ZoneDriver>> m_drivers;
	template<typename T>
	void execute_region_specific(void* regioned_event);

	const uint32_t THIS_SUIT_ID = 0;
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
