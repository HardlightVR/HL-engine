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

		//I'm just going to go ahead and memory leak these for now. 
		//Must wrap the API in c++ objects.
		nsvr_node* newNode;
		nsvr_node_create(&newNode);
		nsvr_node_setdisplayname(newNode, locstring.c_str());

		m_drivers.insert(std::make_pair(
			locstring, 
			std::make_unique<Hardlight_Mk3_ZoneDriver>(Location(loc), newNode))
		);

	}
}




void HardlightDevice::RegisterDrivers(nsvr_core_ctx* ctx)
{
	nsvr_request_callback cevent_handler;
	cevent_handler.user_data = this;

	cevent_handler.handler = [](nsvr_request* event, nsvr_request_type type, void* user_data) {
		HardlightDevice* driver = static_cast<HardlightDevice*>(user_data);
		driver->handle(type, event);
	};

	nsvr_register_request_handler(ctx, 
		nsvr_request_type_brief_haptic, 

		cevent_handler
	);

	nsvr_register_request_handler(ctx,
		nsvr_request_type_lasting_haptic, 
		cevent_handler
	);

	nsvr_register_request_handler(ctx,
		nsvr_request_type_playback_statechange, 
		cevent_handler
	);


	nsvr_register_preset_handler(ctx, [](nsvr_preset_request* req, void* ud) {
		HardlightDevice* hd = static_cast<HardlightDevice*>(ud);
		nsvr_preset_family fam;
		nsvr_preset_request_getfamily(req, &fam);

		std::cout << "Got preset request: family is " << fam << "!\n";
	}, this);
	
}

void HardlightDevice::handle(nsvr_request_type type, nsvr_request * event)
{
	switch (type) {
	case nsvr_request_type_brief_haptic:
		executeBrief(event);
		break;
	case nsvr_request_type_lasting_haptic:
		executeLasting(event);
		break;
	case nsvr_request_type_playback_statechange:
		executePlaybackChange(event);
	//	for (auto& driver : m_drivers) {
		//	driver.second->consume(static_cast<nsvr_request_playback_statechange*>(event));
		//}
		break;
	default:
		break;
	}


}

void HardlightDevice::executeBrief(nsvr_request * event)
{
	

	BasicHapticEventData data = {};
	data.duration = 0.0f;
	nsvr_request_briefhaptic_geteffect(event, &data.effect);
	nsvr_request_briefhaptic_getstrength(event, &data.strength);

	char region[32];
	nsvr_request_briefhaptic_getregion(event, region);

	if (m_drivers.find(region) != m_drivers.end()) {
		m_drivers.at(region)->consumeBrief(std::move(data));
	}
}

void HardlightDevice::executeLasting(nsvr_request * event)
{
	BasicHapticEventData data = {};
	uint64_t id;
	nsvr_request_lastinghaptic_getduration(event, &data.duration);
	nsvr_request_lastinghaptic_geteffect(event, &data.effect);
	nsvr_request_lastinghaptic_getstrength(event, &data.strength);
	nsvr_request_lastinghaptic_getid(event, &id);
	char region[32];
	nsvr_request_briefhaptic_getregion(event, region);
	if (m_drivers.find(region) != m_drivers.end()) {
		m_drivers.at(region)->consumeLasting(std::move(data), id);
	}

}

void HardlightDevice::executePlaybackChange(nsvr_request * event)
{
	uint64_t id;
	nsvr_playback_statechange_command command;
	nsvr_request_playback_statechange_getid(event, &id);
	nsvr_request_playback_statechange_getcommand(event, &command);
	for (auto& driver : m_drivers) {
		driver.second->controlEffect(id, command);
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


