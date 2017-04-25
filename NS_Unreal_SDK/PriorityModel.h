#pragma once
#include <unordered_map>
#include "HapticQueue.h"
#include "IntermediateHapticFormats.h"
#include "SuitHardwareInterface.h"
class PriorityModel
{
public:
	enum Command {
		PLAY, HALT
	};
	struct ExecutionCommand {
		Location Location;
		Effect Effect;
		Duration Duration;
		Command Command;
		ExecutionCommand(::Location loc,  PriorityModel::Command c, ::Duration d = ::Duration::OneShot,::Effect eff = Effect::Buzz_100 ) :Effect(eff),Location(loc), Duration(d), Command(c) {}
	};
	PriorityModel();
	~PriorityModel();
	std::vector<PriorityModel::ExecutionCommand> Update(float dt);
	void Clean(Location loc);
	boost::optional<boost::uuids::uuid> Put(AreaFlag area, HapticEvent e);
	boost::optional<HapticEvent> Remove(AreaFlag area, boost::uuids::uuid e);
private:
	std::unordered_map<Location, HapticQueue> _model;
};

