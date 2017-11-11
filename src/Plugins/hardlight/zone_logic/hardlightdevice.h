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
	void handle_waveform(uint64_t request_id, nsvr_node_id node_id, BasicHapticEventData data);
	void handle_waveform(uint64_t request_id, nsvr_node_id device_id, nsvr_default_waveform waveform, uint32_t reps, float strength);
	void Buffered(uint64_t request_id, nsvr_node_id node_id, const double* amps, uint32_t length);
	void Pause(ParentId handle, nsvr_node_id id);
	void Cancel(ParentId handle,nsvr_node_id id);
	void Unpause(ParentId handle, nsvr_node_id id);

	void EnumerateNodesForDevice(nsvr_node_ids* ids);
	void GetNodeInfo(nsvr_node_id id, nsvr_node_info* info);
	void SetupDeviceAssociations(nsvr_bodygraph* g);
private:

	std::unordered_map<Location, std::unique_ptr<Hardlight_Mk3_ZoneDriver>> m_drivers;
	template<typename T>
	void execute_region_specific(void* regioned_event);

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
