#include "stdafx.h"
#include "MotorStateChanger.h"
#include "Locator.h"
#include "InstructionSet.h"
MotorStateChanger::MotorStateChanger(Location areaId) :
	currentState(MotorFirmwareState::Idle),
	previousContinuous(),
	area(areaId),
	m_lastSample(std::chrono::steady_clock::now())
{
}

MotorStateChanger::MotorFirmwareState MotorStateChanger::GetState() const
{
	return currentState;
}

CommandBuffer MotorStateChanger::transitionTo(LiveBasicHapticEvent & event)
{
	using namespace std::chrono_literals;
	CommandBuffer commands;
	if (event == previousContinuous) {
		//aka, playing another repetition of an effect

		
		if ((std::chrono::steady_clock::now() - m_lastSample) >= event.Duration()) {
			m_lastSample = std::chrono::steady_clock::now();
			const auto& data = event.PollOnce();
			commands.push_back(PlaySingle(static_cast<Location>(data.area), data.effect, data.strength));
		}
	}
	else {
		commands = transition(event.PollOnce());
		previousContinuous = event;
	}

	return commands;
}

CommandBuffer MotorStateChanger::transitionToIdle()
{
	CommandBuffer requiredCmds;
	switch (currentState) {
	case MotorFirmwareState::Idle:
		//do nothing
		break;
	case MotorFirmwareState::PlayingSomething:
	//	requiredCmds.push_back(Halt(area));
		break;
	}

	currentState = MotorFirmwareState::Idle;
	previousContinuous = boost::optional<LiveBasicHapticEvent>();
	return requiredCmds;
}


CommandBuffer MotorStateChanger::transition(const BasicHapticEventData& data)
{
	//Note: as you can see, we generate the same commands from every state.
	//This is subject to change with future firmware versions, which is why it is 
	//setup like this.

	CommandBuffer requiredCmds;
	switch (currentState) {
	case MotorFirmwareState::Idle:
		requiredCmds.push_back(PlaySingle(area, data.effect, data.strength));
		break;
	case MotorFirmwareState::PlayingSomething:
		requiredCmds.push_back(Halt(area));
		requiredCmds.push_back(PlaySingle(area, data.effect, data.strength));
		break;
	}

	currentState = MotorFirmwareState::PlayingSomething;
	return requiredCmds;
}
