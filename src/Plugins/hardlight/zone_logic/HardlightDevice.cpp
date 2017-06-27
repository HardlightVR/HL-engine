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



void HardlightDevice::RegisterDrivers(NSVR_Core* core)
{

	auto& translator = Locator::Translator();
	for (auto& driver : m_drivers) {
		std::string region = translator.ToRegionFromLocation(driver->GetLocation());

		NSVR_Core_RegisterNode2(core, &zoneDriverCallback<NSVR_BriefTaxel>, region.c_str(), "brief-taxel", driver.get());
		NSVR_Core_RegisterNode2(core, &zoneDriverCallback<NSVR_LastingTaxel>, region.c_str(), "lasting-taxel", driver.get());
		NSVR_Core_RegisterNode2(core, &zoneDriverCallback<NSVR_PlaybackEvent>, region.c_str(), "playback-controls", driver.get());
		NSVR_Core_RegisterNode2(core, &zoneDriverCallback<NSVR_RealtimeEvent>, region.c_str(), "realtime", driver.get());

	}
	
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


