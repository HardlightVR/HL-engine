#pragma once
#include "TimeInstant.h"
#include "SuitHardwareInterface.h"
#include "HapticQueue.h"
#include "HapticClasses.h"
#include "IPlayable.h"
#include "PriorityModel.h"
typedef unsigned int HapticHandle;
class HapticsExecutor
{
public:

	HapticsExecutor(std::unique_ptr<SuitHardwareInterface>);
	~HapticsExecutor();

	void Update(float dt);
	void Play(HapticHandle h);
	void Pause(HapticHandle h);
	void Reset(HapticHandle h);
	void Release(HapticHandle h);
	void Create(HapticHandle h, std::unique_ptr<IPlayable> playable);
	const std::unique_ptr<SuitHardwareInterface>& Hardware();
private:

	std::unique_ptr<SuitHardwareInterface> _suit;
	std::unordered_map<HapticHandle, std::unique_ptr<IPlayable>> _effects;
	PriorityModel _model;


	void updateLocationModels(float dt);
};

