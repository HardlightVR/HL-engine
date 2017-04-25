#pragma once
#include "SuitHardwareInterface.h"
#include "PriorityModel.h"

namespace NS {
	namespace Hardware {
		void ExecuteHapticCommands(SuitHardwareInterface& hw, const std::vector<PriorityModel::ExecutionCommand>& commands, PriorityModel& model);
	}
}

