#pragma once
#include "Enums.h"
#include "LiveBasicHapticEvent.h"
#include <boost/optional.hpp>
#include "HardwareCommands.h"

class MotorStateChanger {
public:
	MotorStateChanger(Location areaId);
	enum class MotorFirmwareState {Idle, PlayingSomething};
	MotorFirmwareState GetState() const;
	CommandBuffer transitionTo(LiveBasicHapticEvent& event);
	CommandBuffer transitionToIdle();
private:
	MotorFirmwareState currentState;
	boost::optional<LiveBasicHapticEvent> previousContinuous;
	Location area;
	CommandBuffer transition(const BasicHapticEventData& data);
	std::chrono::time_point<std::chrono::steady_clock> m_lastSample;
};
