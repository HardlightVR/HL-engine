#pragma once
#include "TimeInstant.h"
#include "SuitHardwareInterface.h"
#include "HapticQueue.h"
#include "HapticClasses.h"
#include "IPlayable.h"
#include "PriorityModel.h"
#include "HapticEventGenerator.h"
typedef unsigned int HapticHandle;
class HapticsPlayer
{
public:
	struct Released {
	public:
		boost::uuids::uuid ID;
		bool NeedsSweep;
		Released(boost::uuids::uuid id) :ID(id), NeedsSweep(false) {}
	};

	
	HapticsPlayer(std::shared_ptr<InstructionSet>);
	~HapticsPlayer();

	std::vector<PriorityModel::ExecutionCommand> Update(float dt);
	void Play(HapticHandle h);
	void Pause(HapticHandle h);
	void Restart(HapticHandle h);
	void Stop(HapticHandle h);
	void Release(HapticHandle h);
	void Create(HapticHandle h, std::vector<TinyEffect>);

	PriorityModel& GetModel();

	

	void PlayAll();
	void PauseAll();
	void ClearAll();
	
private:
	boost::hash<boost::uuids::uuid> uuid_hasher;
	boost::uuids::random_generator _uuidGen;
	std::shared_ptr<InstructionSet> _iset;
	std::unordered_map<std::size_t, std::unique_ptr<IPlayable>> _effects;
	PriorityModel _model; //order dependency (1)
	HapticEventGenerator _generator; //order dependency (2)

	std::vector<Released> _releasedEffects;
	std::unordered_map<HapticHandle, boost::uuids::uuid> _outsideHandleToUUID;
	std::vector<std::size_t> _frozenEffects;

	bool _paused;

	
	boost::optional<const std::unique_ptr<IPlayable>&>  toInternalUUID(HapticHandle hh);
};

