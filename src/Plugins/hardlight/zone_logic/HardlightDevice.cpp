#include "stdafx.h"
#include "HardlightDevice.h"
#include <boost/uuid/random_generator.hpp>
#include <experimental/vector>
#include "PluginAPI.h"
#include "Locator.h"



HardlightDevice::HardlightDevice() 
{
	auto& translator = Locator::Translator();

	for (int loc = (int)Location::Lower_Ab_Right; loc != (int)Location::Error; loc++) {

		m_drivers.insert(std::make_pair(
			(Location)loc, 
			std::make_unique<Hardlight_Mk3_ZoneDriver>(Location(loc)))
		);
	}
}




void HardlightDevice::Configure(nsvr_core* ctx)
{


	nsvr_plugin_waveform_api waveform_api;
	waveform_api.client_data = this;
	waveform_api.activate_handler = [](uint64_t request_id, nsvr_node_id device_id, nsvr_waveform* waveform, void* cd) {
		AS_TYPE(HardlightDevice, cd)->handle(request_id, device_id, waveform);
	};

	nsvr_register_waveform_api(ctx, &waveform_api);

	nsvr_plugin_playback_api playback_api;
	playback_api.client_data = this;
	playback_api.pause_handler = [](uint64_t handle, void* client_data) {
		AS_TYPE(HardlightDevice, client_data)->Pause(handle);
	};
	playback_api.cancel_handler = [](uint64_t handle, void* client_data) {
		AS_TYPE(HardlightDevice, client_data)->Cancel(handle);
	};
	playback_api.unpause_handler = [](uint64_t handle, void* client_data) {
		AS_TYPE(HardlightDevice, client_data)->Unpause(handle);
	};

	nsvr_register_playback_api(ctx, &playback_api);

	nsvr_plugin_buffered_api buffered_api;
	buffered_api.client_data = this;
	buffered_api.getmaxsamples_handler = [](uint32_t* outMaxSamples, void* cd) {
		*outMaxSamples = 512;
	};
	buffered_api.getsampleduration_handler = [](double* outDuration, void* cd) {
		*outDuration = 0.25;
	};
	buffered_api.submit_handler = [](uint64_t request_id, nsvr_node_id id, double* amplitudes, uint32_t count, void* cd) {
		AS_TYPE(HardlightDevice, cd)->Buffered(request_id, id, amplitudes, count);
	};
	nsvr_register_buffered_api(ctx, &buffered_api);

	
	nsvr_plugin_device_api device_api;
	device_api.client_data = this;
	device_api.enumeratenodes_handler = [](uint32_t device_id, nsvr_node_ids* ids, void* cd) {
		AS_TYPE(HardlightDevice, cd)->EnumerateNodesForDevice(device_id, ids);
	};
	device_api.enumeratedevices_handler = [](nsvr_device_ids* ids, void* cd) {
		AS_TYPE(HardlightDevice, cd)->EnumerateDevices(ids);
	};
	device_api.getdeviceinfo_handler = [](uint32_t id, nsvr_device_info* info, void* cd) {
		AS_TYPE(HardlightDevice, cd)->GetDeviceInfo(id, info);
	};

	device_api.getnodeinfo_handler = [](uint64_t id, nsvr_node_info* info, void* cd) {
		AS_TYPE(HardlightDevice, cd)->GetNodeInfo(id, info);
	};
	nsvr_register_device_api(ctx, &device_api);
	
}



void HardlightDevice::Pause(ParentId handle)
{
	for (auto& driver : m_drivers) {
		driver.second->controlEffect(handle, 1);
	}
}

void HardlightDevice::Cancel(ParentId  handle)
{
	for (auto& driver : m_drivers) {
		driver.second->controlEffect(handle, 3);
	}
}

void HardlightDevice::Unpause(ParentId  handle)
{
	for (auto& driver : m_drivers) {
		driver.second->controlEffect(handle, 2);
	}
}


void HardlightDevice::EnumerateNodesForDevice(uint32_t device_id, nsvr_node_ids* ids)
{
	std::vector<uint64_t> found_ids;
	for (const auto& device : m_drivers) {
		found_ids.push_back(device.second->GetId());
	}
	
	for (std::size_t i = 0; i < found_ids.size(); i++) {
		ids->ids[i] = found_ids[i];
	}

	ids->node_count = found_ids.size();
}

void HardlightDevice::EnumerateDevices(nsvr_device_ids * ids)
{
	//if (this->EnumerateNodesForDevice
	ids->device_count = 1;
	ids->ids[0] = THIS_SUIT_ID;
}

void HardlightDevice::GetNodeInfo(uint64_t id, nsvr_node_info* info) {
	const auto& t = Locator::Translator();
	auto it = std::find_if(m_drivers.begin(), m_drivers.end(), [id = id](const auto& driver) {
		return driver.second->GetId() == id;
	});

	if (it != m_drivers.end()) {
		info->capabilities = nsvr_device_capability_dynamic;
		info->type = nsvr_node_type_haptic;
		info->id = id;

		const auto& driver = it->second;
		std::string outStr = "Hardlight ZoneDriver " + t.ToString(driver->GetLocation());
		std::copy(outStr.begin(), outStr.end(), info->name);

	}
}
void HardlightDevice::GetDeviceInfo(uint32_t id, nsvr_device_info* info)
{
	info->id = THIS_SUIT_ID;
	
	std::string name("Hardlight Suit");
	std::copy(name.begin(), name.end(), info->name);

}


void HardlightDevice::RaiseDeviceConnectionEvent(nsvr_core* core)
{
		nsvr_device_event_raise(core, nsvr_device_event_device_connected, THIS_SUIT_ID);
	
}

void HardlightDevice::RaiseDeviceDisconnectionEvent(nsvr_core* core)
{
	
		nsvr_device_event_raise(core, nsvr_device_event_device_disconnected, THIS_SUIT_ID);
	
}

void HardlightDevice::SetupDeviceAssociations(nsvr_bodygraph* g)
{
	nsvr_bodygraph_associate(g, "upperChestActuators:0", m_drivers[Location::Chest_Left]->GetId());
	nsvr_bodygraph_associate(g, "leftAbActuators:0", m_drivers[Location::Upper_Ab_Left]->GetId());
	nsvr_bodygraph_associate(g, "leftAbActuators:1", m_drivers[Location::Mid_Ab_Left]->GetId());
	nsvr_bodygraph_associate(g, "leftAbActuators:2", m_drivers[Location::Lower_Ab_Left]->GetId());

	nsvr_bodygraph_associate(g, "leftShoulderActuator", m_drivers[Location::Shoulder_Left]->GetId());
	nsvr_bodygraph_associate(g, "leftUpperArmActuator", m_drivers[Location::Upper_Arm_Left]->GetId());
	nsvr_bodygraph_associate(g, "leftLowerArmActuator", m_drivers[Location::Forearm_Left]->GetId());

	nsvr_bodygraph_associate(g, "upperBackActuators:0", m_drivers[Location::Upper_Back_Left]->GetId());


	nsvr_bodygraph_associate(g, "upperChestActuators:1", m_drivers[Location::Chest_Right]->GetId());
	nsvr_bodygraph_associate(g, "rightAbActuators:0", m_drivers[Location::Upper_Ab_Right]->GetId());
	nsvr_bodygraph_associate(g, "rightAbActuators:1", m_drivers[Location::Mid_Ab_Right]->GetId());
	nsvr_bodygraph_associate(g, "rightAbActuators:2", m_drivers[Location::Lower_Ab_Right]->GetId());

	nsvr_bodygraph_associate(g, "rightShoulderActuator", m_drivers[Location::Shoulder_Right]->GetId());
	nsvr_bodygraph_associate(g, "rightUpperArmActuator", m_drivers[Location::Upper_Arm_Right]->GetId());
	nsvr_bodygraph_associate(g, "rightLowerArmActuator", m_drivers[Location::Forearm_Right]->GetId());

	nsvr_bodygraph_associate(g, "upperBackActuators:1", m_drivers[Location::Upper_Back_Right]->GetId());

}

void HardlightDevice::handle(uint64_t request_id, uint64_t device_id, nsvr_waveform* wave) {
	auto it = std::find_if(m_drivers.begin(), m_drivers.end(), [device_id](const auto& driver) { return driver.second->GetId() == device_id; });
	if (it != m_drivers.end()) {
		BasicHapticEventData data = {};
		nsvr_default_waveform stuff;
		nsvr_waveform_getname(wave, &stuff);
		data.effect = stuff;
		nsvr_waveform_getstrength(wave, &data.strength);

		float duration = 0;
		uint32_t repetitions = 0;
		nsvr_waveform_getrepetitions(wave, &repetitions);
		if (repetitions > 0) {
			duration = 0.25f * repetitions;
		}
		data.duration = duration;
		(*it).second->consumeLasting(std::move(data), request_id);
	}
}
void HardlightDevice::Buffered(uint64_t request_id, nsvr_node_id node_id, double * amps, uint32_t length)
{
	auto it = std::find_if(m_drivers.begin(), m_drivers.end(), [device_id = node_id](const auto& driver) { return driver.second->GetId() == device_id; });
	if (it != m_drivers.end()) {
		(*it).second->realtime(std::vector<double>(amps, amps + length), request_id);
	}
}
CommandBuffer HardlightDevice::GenerateHardwareCommands(float dt)
{
	CommandBuffer result;
	for (auto& driver_pair : m_drivers) {
		CommandBuffer cl = driver_pair.second->update(dt);
		result.insert(result.begin(), cl.begin(), cl.end());
	}
	return result;
		
}



//DisplayResults HardlightDevice::QueryDrivers()
//{
//	DisplayResults representations;
//
//	for (const auto& driver : m_drivers) {
//		auto result = driver->QueryCurrentlyPlaying();
//		if (result) {
//			representations.push_back(*result);
//		}
//	}
//
//	return representations;
//}
//


ZoneModel::UserCommand::UserCommand(): id(0), command(Command::Unknown)
{
}

ZoneModel::UserCommand::UserCommand(ParentId id, Command c) : id(id), command(c)
{
}


