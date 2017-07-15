#include "stdafx.h"
#include "HardlightMk3ZoneDriver.h"
#include "Locator.h"
#include <boost/uuid/random_generator.hpp>
#include <limits>
CommandBuffer Hardlight_Mk3_ZoneDriver::update(float dt)
{
	//think about if the commandbuffer vectors should really be reversed
	auto rtpCommands = m_rtpModel.Update(dt);
	auto retainedCommands = m_retainedModel.Update(dt);

	
	std::lock_guard<std::mutex> guard(m_mutex);

	CommandBuffer result;
	result.swap(m_commands);

	if (m_currentMode == Mode::Realtime) {
		result.insert(result.end(), rtpCommands.begin(), rtpCommands.end());
	}
	else {
		result.insert(result.end(), retainedCommands.begin(), retainedCommands.end());
	}

	return result;

}



Hardlight_Mk3_ZoneDriver::Hardlight_Mk3_ZoneDriver(::Location area) :
	m_area(area),
	m_currentMode(Mode::Retained),
	m_commands(),
	m_rtpModel(m_area),
	m_retainedModel(m_area),
	m_mutex()
{

}

::Location Hardlight_Mk3_ZoneDriver::GetLocation()
{
	return static_cast<::Location>(m_area);
}



//boost::optional<HapticDisplayInfo> Hardlight_Mk3_ZoneDriver::QueryCurrentlyPlaying()
//{
//	auto& translator = Locator::getTranslator();
//
//	if (m_currentMode == Mode::Retained) {
//		auto potentialEvent = m_retainedModel.GetCurrentlyPlayingEvent();
//		if (potentialEvent) {
//			HapticDisplayInfo info;
//			info.area = translator.ToArea(m_area);
//			info.family = potentialEvent->Data().effect;
//			info.strength = static_cast<uint16_t>(255 * potentialEvent->Data().strength);
//			return info;
//		}
//	}
//	else {
//		HapticDisplayInfo info;
//		info.area = translator.ToArea(m_area);
//		info.family = 0;
//		info.strength = m_rtpModel.GetVolume();
//		return info;
//	}
//
//	return boost::optional<HapticDisplayInfo>();
//}

//void Hardlight_Mk3_ZoneDriver::realtime(const RealtimeArgs& args)
//{
//	m_rtpModel.ChangeVolume(args.volume);
//	transitionInto(Mode::Realtime);
//}

void Hardlight_Mk3_ZoneDriver::transitionInto(Mode mode)
{
	std::lock_guard<std::mutex> guard(m_mutex);

	if (mode == Mode::Retained) {
		m_currentMode = Mode::Retained;

		m_commands.push_back(EnableIntrig(m_area));
	}
	else if (mode == Mode::Realtime) {
		m_currentMode = Mode::Realtime;
		m_commands.push_back(EnableRtp(m_area));
		
	}
}

void Hardlight_Mk3_ZoneDriver::consume(const nsvr_cevent_brief_haptic * event)
{
	BasicHapticEventData d;
	d.area = static_cast<uint32_t>(m_area); //areaFlag at this point
	d.effect = event->effect;
	d.strength = event->strength;
	d.duration = 0.0;
	m_retainedModel.Put(LiveBasicHapticEvent(std::numeric_limits<ParentId>::max(), m_gen(), std::move(d)));
	transitionInto(Mode::Retained);
}


void Hardlight_Mk3_ZoneDriver::consume(const nsvr_cevent_lasting_haptic * haptic)
{
	BasicHapticEventData d;
	uint64_t id;
	d.area = static_cast<uint32_t>(m_area); //areaFlag at this point
	id = haptic->id;
	d.effect = haptic->effect;
	d.strength = haptic->strength;
	d.duration = haptic->duration;

	m_retainedModel.Put(LiveBasicHapticEvent(id, m_gen(), std::move(d)));
	transitionInto(Mode::Retained);
}

void Hardlight_Mk3_ZoneDriver::consume(const nsvr_cevent_playback_statechange * event)
{
	uint64_t id = event->effect_id;
	auto command = event->command;


	switch (command) {
	case nsvr_playback_statechange_unpause:
		m_retainedModel.Play(id);
		break;
	case nsvr_playback_statechange_pause:
		m_retainedModel.Pause(id);
		break;
	case nsvr_playback_statechange_cancel:
		m_retainedModel.Remove(id);
	default:
		break;
	}

}

	



void Hardlight_Mk3_ZoneDriver::consume(const NSVR_RealtimeEvent* realtime)
{

	float volume = 0;
	NSVR_RealtimeEvent_GetStrength(realtime, &volume);
	m_rtpModel.ChangeVolume(static_cast<int>(volume * 255));
	transitionInto(Mode::Realtime);
}

//void Hardlight_Mk3_ZoneDriver::controlRetained(boost::uuids::uuid handle, NSVR_PlaybackCommand command)
//{
//
//	switch (command) {
//	case NSVR_PlaybackCommand::NSVR_PlaybackCommand_Play:
//		m_retainedModel.Play(handle);
//		break;
//	case NSVR_PlaybackCommand::NSVR_PlaybackCommand_Pause:
//		m_retainedModel.Pause(handle);
//		break;
//	case NSVR_PlaybackCommand::NSVR_PlaybackCommand_Reset:
//		m_retainedModel.Remove(handle);
//	default:
//		break;
//	}
//}