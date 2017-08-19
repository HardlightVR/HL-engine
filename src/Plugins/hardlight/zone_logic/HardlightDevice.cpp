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
		nsvr_region region = nsvr_region::test;

		m_drivers.insert(std::make_pair(
			region, 
			std::make_unique<Hardlight_Mk3_ZoneDriver>(Location(loc)))
		);
		//todo: FIX THIS
		// REGIONS AREN'T REAL!
	}
}




void HardlightDevice::Configure(nsvr_core* ctx)
{


	nsvr_plugin_waveform_api waveform_api;
	waveform_api.client_data = this;
	waveform_api.activate_handler = [](uint64_t request_id, uint64_t device_id, nsvr_waveform* waveform, void* cd) {
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



	nsvr_plugin_sampling_api sampling_api;
	sampling_api.client_data = this;
	sampling_api.query_handler = [](nsvr_region node, nsvr_sampling_sample* outState, void* client_data) {
		AS_TYPE(HardlightDevice, client_data)->Query(node, outState);
	};

	nsvr_register_sampling_api(ctx, &sampling_api);

	
	nsvr_plugin_device_api device_api;
	device_api.client_data = this;
	device_api.enumerateids_handler = [](nsvr_device_ids* ids, void* cd) {
		AS_TYPE(HardlightDevice, cd)->EnumerateDeviceIds(ids);
	};
	device_api.getinfo_handler = [](uint64_t id, nsvr_device_basic_info* info, void* cd) {
		AS_TYPE(HardlightDevice, cd)->GetDeviceInfo(id, info);
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

int HardlightDevice::Query(nsvr_region node, nsvr_sampling_sample * outState)
{
	if (m_drivers.find(node) != m_drivers.end()) {
		if (m_drivers.at(node)->IsPlaying()) {

			outState->intensity = 1;
		} 
		else {
			outState->intensity = 0;
		}
		return 1;
	}

	return 0;

}

void HardlightDevice::EnumerateDeviceIds(nsvr_device_ids * ids)
{
	std::vector<uint64_t> found_ids;
	for (const auto& device : m_drivers) {
		found_ids.push_back(device.second->GetId());
	}
	
	for (std::size_t i = 0; i < found_ids.size(); i++) {
		ids->ids[i] = found_ids[i];
	}

	ids->device_count = found_ids.size();
}


void HardlightDevice::GetDeviceInfo(uint64_t id, nsvr_device_basic_info* info)
{
	const auto& t = Locator::Translator();
	auto it = std::find_if(m_drivers.begin(), m_drivers.end(), [id = id](const auto& driver) {
		return driver.second->GetId() == id;
	});

	if (it != m_drivers.end()) {
		info->capabilities = nsvr_device_capability_dynamic;
		info->type = nsvr_device_type_haptic;
		info->id = id;
		info->region = it->first;

		const auto& driver = it->second;
		std::string outStr = "Hardlight ZoneDriver " + t.ToString(driver->GetLocation());
		std::copy(outStr.begin(), outStr.end(), info->name);
		
	}
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


