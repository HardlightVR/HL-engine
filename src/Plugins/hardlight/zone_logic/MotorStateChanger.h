#pragma once
#include "Enums.h"
#include "LiveBasicHapticEvent.h"
#include <boost/optional.hpp>
#include "HardwareCommands.h"

class MotorStateChanger {
public:
	MotorStateChanger(Location areaId);
	enum class MotorFirmwareState {Idle, PlayingOneshot, PlayingContinuous};
	MotorFirmwareState GetState() const;
	CommandBuffer transitionTo(const LiveBasicHapticEvent& event);
	CommandBuffer transitionToIdle();
private:
	MotorFirmwareState currentState;
	boost::optional<LiveBasicHapticEvent> previousContinuous;
	Location area;
	CommandBuffer transitionToOneshot(BasicHapticEventData data);
	CommandBuffer transitionToContinuous(BasicHapticEventData data);
};
