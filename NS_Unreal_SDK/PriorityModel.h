#pragma once
#include <unordered_map>
#include "HapticQueue.h"
#include "IntermediateHapticFormats.h"

class SuitHardwareInterface;
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
	PriorityModel(std::unique_ptr<SuitHardwareInterface>& _hardware);
	~PriorityModel();
	std::vector<PriorityModel::ExecutionCommand> Update(float dt);
	void Clean(Location loc);
	boost::optional<boost::uuids::uuid> Put(AreaFlag area, HapticEvent e);
	boost::optional<HapticEvent> Remove(AreaFlag area, boost::uuids::uuid e);
private:
	std::unordered_map<Location, HapticQueue> _model;
	std::unique_ptr<SuitHardwareInterface>& _hardware;
};

