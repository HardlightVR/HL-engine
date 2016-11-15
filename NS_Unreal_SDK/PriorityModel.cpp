#include "stdafx.h"
#include "PriorityModel.h"
#include "SuitHardwareInterface.h"
#include "HapticEvent.h"

PriorityModel::PriorityModel(std::unique_ptr<SuitHardwareInterface>& h):_hardware(h)
{
}


PriorityModel::~PriorityModel()
{
}

#define START_BITMASK_SWITCH(x) \
for (uint32_t bit = 1; x >= bit; bit *=2) if (x & bit) switch(AreaFlag(bit))




std::vector<PriorityModel::ExecutionCommand> PriorityModel::Update(float dt)
{
	for (auto& queue : _model) {
		queue.second.Update(dt);
	}

	std::vector<ExecutionCommand> commands;
	for (auto& queue : _model) {
		HapticEvent* effect = _model[queue.first].GetNextEvent();
		if (effect == nullptr)
		{
			if (queue.second.Dirty)
			{
				commands.push_back(ExecutionCommand(queue.first, PriorityModel::Command::HALT));
				//_hardware->HaltEffect(queue.first);
				_model[queue.first].Dirty = false;
			}
			continue;
		}
		else {
			//BUG: Need to not send continuous play repeatedly 

			if (!effect->Sent) {
				effect->Sent = true;
				Duration d = Duration(effect->DurationType());
				
				commands.push_back(ExecutionCommand(queue.first, PriorityModel::Command::PLAY, effect->DurationType(), effect->Effect));
			}
		}
	}
	return commands;

}

void PriorityModel::Clean(Location loc)
{
	_model[loc].Dirty = false;

}


boost::optional<HapticEvent> PriorityModel::Remove(AreaFlag area, boost::uuids::uuid e)
{
	boost::optional<HapticEvent> removedEffect;
	START_BITMASK_SWITCH(uint32_t(area))
	{
		case AreaFlag::Forearm_Left:
			removedEffect = _model[Location::Forearm_Left].Remove(e);
			break;
		case AreaFlag::Upper_Arm_Left:
			removedEffect = _model[Location::Upper_Arm_Left].Remove(e);
			break;
		case AreaFlag::Shoulder_Left:
			removedEffect = _model[Location::Shoulder_Left].Remove(e);
			break;
		case AreaFlag::Back_Left:
			removedEffect = _model[Location::Upper_Back_Left].Remove(e);
			break;
		case AreaFlag::Chest_Left:
			removedEffect = _model[Location::Chest_Left].Remove(e);
			break;
		case AreaFlag::Upper_Ab_Left:
			removedEffect = _model[Location::Upper_Ab_Left].Remove(e);
			break;
		case AreaFlag::Mid_Ab_Left:
			removedEffect = _model[Location::Mid_Ab_Left].Remove(e);
			break;
		case AreaFlag::Lower_Ab_Left:
			removedEffect = _model[Location::Lower_Ab_Left].Remove(e);
			break;
		case AreaFlag::Forearm_Right:
			removedEffect = _model[Location::Forearm_Right].Remove(e);
			break;
		case AreaFlag::Upper_Arm_Right:
			removedEffect = _model[Location::Upper_Arm_Right].Remove(e);
			break;
		case AreaFlag::Shoulder_Right:
			removedEffect = _model[Location::Shoulder_Right].Remove(e);
			break;
		case AreaFlag::Back_Right:
			removedEffect = _model[Location::Upper_Back_Right].Remove(e);
			break;
		case AreaFlag::Chest_Right:
			removedEffect = _model[Location::Chest_Right].Remove(e);
			break;
		case AreaFlag::Upper_Ab_Right:
			removedEffect = _model[Location::Upper_Ab_Right].Remove(e);
			break;
		case AreaFlag::Mid_Ab_Right:
			removedEffect = _model[Location::Mid_Ab_Right].Remove(e);
			break;
		case AreaFlag::Lower_Ab_Right:
			removedEffect = _model[Location::Lower_Ab_Right].Remove(e);
			break;
		default:
			break;
	}

	return removedEffect;
}



boost::optional<boost::uuids::uuid> PriorityModel::Put(AreaFlag area, HapticEvent e)
{

	START_BITMASK_SWITCH(uint32_t(area)) 
	{
		case AreaFlag::Forearm_Left:
			_model[Location::Forearm_Left].Put(1, e);
			break;
		case AreaFlag::Upper_Arm_Left:
			_model[Location::Upper_Arm_Left].Put(1, e);
			break;
		case AreaFlag::Shoulder_Left:
			_model[Location::Shoulder_Left].Put(1, e);
			break;
		case AreaFlag::Back_Left:
			_model[Location::Upper_Back_Left].Put(1, e);
			break;
		case AreaFlag::Chest_Left:
			_model[Location::Chest_Left].Put(1, e);
			break;
		case AreaFlag::Upper_Ab_Left:
			_model[Location::Upper_Ab_Left].Put(1, e);
			break;
		case AreaFlag::Mid_Ab_Left:
			_model[Location::Mid_Ab_Left].Put(1, e);
			break;
		case AreaFlag::Lower_Ab_Left:
			_model[Location::Lower_Ab_Left].Put(1, e);
			break;
		case AreaFlag::Forearm_Right:
			_model[Location::Forearm_Right].Put(1, e);
			break;
		case AreaFlag::Upper_Arm_Right:
			_model[Location::Upper_Arm_Right].Put(1, e);
			break;
		case AreaFlag::Shoulder_Right:
			_model[Location::Shoulder_Right].Put(1, e);
			break;
		case AreaFlag::Back_Right:
			_model[Location::Upper_Back_Right].Put(1, e);
			break;
		case AreaFlag::Chest_Right:
			_model[Location::Chest_Right].Put(1, e);
			break;
		case AreaFlag::Upper_Ab_Right:
			_model[Location::Upper_Ab_Right].Put(1, e);
			break;
		case AreaFlag::Mid_Ab_Right:
			_model[Location::Mid_Ab_Right].Put(1, e);
			break;
		case AreaFlag::Lower_Ab_Right:
			_model[Location::Lower_Ab_Right].Put(1, e);
			break;
		default:
			break;
	};
	return e.Handle;
}


