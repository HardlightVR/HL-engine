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
	for (int loc = (int)Location::Lower_Ab_Right; loc != (int)Location::Error; loc++) {
		m_drivers.push_back(std::make_unique<Hardlight_Mk3_ZoneDriver>(Location(loc)));
	}
}

struct driver_with_region {
	Hardlight_Mk3_ZoneDriver* driver;
	const char* region;
};

struct brief_haptic {
	float duration;
};

struct lasting_haptic {

};
struct playback_control {

};




void handle_event(void* event,  nsvr_cevent_type type,  void* user_data) {
	Hardlight_Mk3_ZoneDriver* driver = static_cast<Hardlight_Mk3_ZoneDriver*>(user_data);


	switch (type) {
	case nsvr_cevent_type_brief_haptic:
		driver->consume(static_cast<nsvr_cevent_brief_haptic*>(event));
		break;
	case nsvr_cevent_type_lasting_haptic:
		driver->consume(static_cast<nsvr_cevent_lasting_haptic*>(event));
		break;
	case nsvr_cevent_type_playback_statechange:
		driver->consume(static_cast<nsvr_cevent_playback_statechange*>(event));
		break;
	default:
		break;
	}
	
	
}
void HardlightDevice::RegisterDrivers(nsvr_core_ctx* ctx)
{
	nsvr_cevent_callback briefCallback;

	briefCallback.handler = handle_event;
	briefCallback.targetVersion = nsvr_cevent_brief_haptic_latest;
	briefCallback.user_data = this;

	nsvr_register_cevent_hook(ctx, nsvr_cevent_type_brief_haptic, briefCallback);

	/*auto& translator = Locator::Translator();
	for (auto& driver : m_drivers) {
		std::string region = translator.ToRegionFromLocation(driver->GetLocation());
		const char* region_str = region.c_str();
		Hardlight_Mk3_ZoneDriver* driver_ptr = driver.get();
		

		registerFunc(&makeCallback<NSVR_BriefTaxel>, region_str, "brief-taxel", driver_ptr);
		registerFunc(&makeCallback<NSVR_LastingTaxel>, region_str, "lasting-taxel", driver_ptr);
		registerFunc(&makeCallback<NSVR_PlaybackEvent>, region_str, "playback-controls", driver_ptr);
		registerFunc(&makeCallback<NSVR_RealtimeEvent>, region_str, "realtime", driver_ptr);

	}*/
	
}
void HardlightDevice::handle(nsvr_cevent * event)
{
	//nsvr_cevent_getregion(event);
	//m_drivers.at(region)->briefHaptic(event);


}

CommandBuffer HardlightDevice::GenerateHardwareCommands(float dt)
{
	CommandBuffer result;
	for (auto& driver : m_drivers) {
		CommandBuffer cl = driver->update(dt);
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


