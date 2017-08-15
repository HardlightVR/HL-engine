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



bool Hardlight_Mk3_ZoneDriver::IsPlaying()
{
	if (m_currentMode == Mode::Retained) {
		if (auto event = m_retainedModel.GetCurrentlyPlayingEvent()) {
			return true;
		}
	}
	else {
		if (m_rtpModel.GetVolume() > 0) {
			return true;
		}
	}

	return false;
}

uint64_t Hardlight_Mk3_ZoneDriver::GetId()
{
	//todo: fix
	//this is temporary. Need to decide if IDs should be random? The same at startup?
	//Follow an order? We should make a guideline for the platform and follow it.
	return static_cast<uint64_t>(m_area);
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





void Hardlight_Mk3_ZoneDriver::consumeLasting(BasicHapticEventData data,ParentId id) {
	data.area = static_cast<uint32_t>(m_area);
	m_retainedModel.Put(LiveBasicHapticEvent(id, m_gen(), std::move(data)));
	transitionInto(Mode::Retained);
}

void Hardlight_Mk3_ZoneDriver::controlEffect(ParentId handle, int command)
{

	switch (command) {
	case 1:
		m_retainedModel.Pause(handle);
		break;
	case 2:
		m_retainedModel.Play(handle);
		break;
	case 3:
		m_retainedModel.Remove(handle);
	default:
		break;
	}
}




//void Hardlight_Mk3_ZoneDriver::consume(const NSVR_RealtimeEvent* realtime)
//{
//
//	float volume = 0;
//	NSVR_RealtimeEvent_GetStrength(realtime, &volume);
//	m_rtpModel.ChangeVolume(static_cast<int>(volume * 255));
//	transitionInto(Mode::Realtime);
//}

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