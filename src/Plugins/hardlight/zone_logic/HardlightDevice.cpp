#include "stdafx.h"
#include "HardlightDevice.h"
#include <boost/uuid/random_generator.hpp>
#include <experimental/vector>
#include "PluginAPI.h"
#include "Locator.h"
#include "../include/events/BriefTaxel.h"
#include "../include/events/RealtimeEvent.h"


HardlightDevice::HardlightDevice() 
{
	auto& translator = Locator::Translator();
	for (int loc = (int)Location::Lower_Ab_Right; loc != (int)Location::Error; loc++) {
		std::string locstring = translator.ToRegionFromLocation(Location(loc));
		m_drivers.insert(std::make_pair(
			locstring, 
			std::make_unique<Hardlight_Mk3_ZoneDriver>(Location(loc)))
		);
	}
}




void HardlightDevice::RegisterDrivers(nsvr_core_ctx* ctx)
{
	nsvr_cevent_callback cevent_handler;
	cevent_handler.user_data = this;

	cevent_handler.handler = [](void* event, nsvr_cevent_type type, void* user_data) {
		HardlightDevice* driver = static_cast<HardlightDevice*>(user_data);
		driver->handle(type, event);
	};

	nsvr_register_cevent_hook(ctx, 
		nsvr_cevent_type_brief_haptic, 
		nsvr_cevent_brief_haptic_latest, 
		cevent_handler
	);

	nsvr_register_cevent_hook(ctx, 
		nsvr_cevent_type_lasting_haptic, 
		nsvr_cevent_lasting_haptic_latest, 
		cevent_handler
	);

	nsvr_register_cevent_hook(ctx, 
		nsvr_cevent_type_playback_statechange, 
		nsvr_cevent_playback_statechange_latest,
		cevent_handler
	);
	
}

void HardlightDevice::handle(nsvr_cevent_type type, void * event)
{
	switch (type) {
	case nsvr_cevent_type_brief_haptic:
		execute_region_specific<nsvr_cevent_brief_haptic>(event);
		break;
	case nsvr_cevent_type_lasting_haptic:
		execute_region_specific<nsvr_cevent_lasting_haptic>(event);
		break;
	case nsvr_cevent_type_playback_statechange:
		for (auto& driver : m_drivers) {
			driver.second->consume(static_cast<nsvr_cevent_playback_statechange*>(event));
		}
		break;
	default:
		break;
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


ZoneModel::UserCommand::UserCommand(): id(), command(Command::Unknown)
{
}

ZoneModel::UserCommand::UserCommand(ParentId id, Command c) : id(id), command(c)
{
}


