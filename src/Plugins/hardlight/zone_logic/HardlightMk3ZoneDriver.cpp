#include "stdafx.h"
#include "HardlightMk3ZoneDriver.h"
#include <limits>
#include "Locator.h"
#include "InstructionSet.h"
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



Hardlight_Mk3_ZoneDriver::Hardlight_Mk3_ZoneDriver(::Location area, const InstructionSet& instructions) :
	m_area(area),
	m_currentMode(Mode::Continuous),
	m_commands(),
	m_rtpModel(m_area),
	m_retainedModel(m_area),
	m_mutex(),
	m_instructions(instructions)
{

}

::Location Hardlight_Mk3_ZoneDriver::GetLocation()
{
	return static_cast<::Location>(m_area);
}



bool Hardlight_Mk3_ZoneDriver::IsPlaying()
{
	if (m_currentMode == Mode::Continuous) {
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

uint32_t Hardlight_Mk3_ZoneDriver::GetId()
{
	//todo: fix
	//this is temporary. Need to decide if IDs should be random? The same at startup?
	//Follow an order? We should make a guideline for the platform and follow it.
	return static_cast<uint32_t>(m_area);
}



void Hardlight_Mk3_ZoneDriver::transitionInto(Mode mode)
{
	std::lock_guard<std::mutex> guard(m_mutex);

	if (m_currentMode == Mode::Continuous) {
		if (mode == Mode::Realtime) {
			m_currentMode = Mode::Realtime;
			m_commands.push_back(EnableRtp(m_area));
		}
	}
	else if (m_currentMode == Mode::Realtime) {
		if (mode == Mode::Continuous) {
			m_currentMode = Mode::Continuous;
			m_commands.push_back(EnableIntrig(m_area));
		}
	}
	else {
		//unknown mode..?
	}
	
}





void Hardlight_Mk3_ZoneDriver::consumeLasting(BasicHapticEventData data, ParentId id) {
	data.area = static_cast<uint32_t>(m_area);
	transitionInto(Mode::Continuous);

	std::string effectString = Locator::Translator().ToString(data.effect);
	const auto& atoms = m_instructions.Atoms();
	const auto it = atoms.find(effectString);

	if (it != atoms.end()) {
		int duration = it->second.GetDuration();
		m_retainedModel.Put(LiveBasicHapticEvent(id, m_gen(), std::move(data), std::chrono::milliseconds(duration)));
	}
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




void Hardlight_Mk3_ZoneDriver::realtime(std::vector<double> samples, ParentId id)
{
	transitionInto(Mode::Realtime);

	m_rtpModel.Put(BufferedEvent(id, m_gen(), std::move(samples)));
}

