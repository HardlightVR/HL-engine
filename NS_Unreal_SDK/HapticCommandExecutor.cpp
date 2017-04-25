#include "HapticCommandExecutor.h"



void NS::Hardware::ExecuteHapticCommands(SuitHardwareInterface & hw, const std::vector<PriorityModel::ExecutionCommand>& commands, PriorityModel & model)
{

	for (auto& command : commands)
	{
		switch (command.Command) {
		case PriorityModel::Command::HALT:
			hw.HaltEffect(command.Location);
			break;
		case PriorityModel::Command::PLAY:
			switch (command.Duration) {
			case Duration::Infinite:
			case Duration::Variable:
				hw.PlayEffectContinuous(command.Location, command.Effect);
				break;
			case Duration::OneShot:
				hw.HaltEffect(command.Location);
				hw.PlayEffect(command.Location, command.Effect);
				model.Clean(command.Location);
				break;
			default:
				break;
			}
		}
	}
}
