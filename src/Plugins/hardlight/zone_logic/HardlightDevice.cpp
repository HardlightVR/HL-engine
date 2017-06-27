#include "stdafx.h"
#include "HardlightDevice.h"
#include <boost/uuid/random_generator.hpp>
#include <experimental/vector>
#include "PluginAPI.h"
#include "Locator.h"




HardlightDevice::HardlightDevice() 
{
	for (int loc = (int)Location::Lower_Ab_Right; loc != (int)Location::Error; loc++) {
		m_drivers.push_back(std::make_unique<Hardlight_Mk3_ZoneDriver>(Location(loc)));
	}
}

void HardlightDevice::RegisterDrivers(NSVR_Core* core)
{
	NSVR_RegParams params = { 0 };
	params.Interface = "brief-taxel";

	auto& translator = Locator::Translator();
	for (auto& driver : m_drivers) {
		std::string region = translator.ToRegionFromLocation(driver->GetLocation());
		params.Region = region.c_str();
		params.Provider = AS_TYPE(NSVR_Provider, driver.get());
		NSVR_Core_RegisterNode(core, params);
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


