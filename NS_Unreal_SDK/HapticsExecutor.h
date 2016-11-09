#pragma once
#include "TimeInstant.h"
#include "SuitHardwareInterface.h"
#include "HapticQueue.h"
#include "HapticClasses.h"
#include "IPlayable.h"

typedef unsigned int HapticHandle;
class HapticsExecutor
{
public:

	HapticsExecutor(std::unique_ptr<SuitHardwareInterface>);
	~HapticsExecutor();
	void Play(const std::vector<HapticFrame>& frames);
	void Play(std::vector<HapticEffect> effects);
	void Play(const std::vector<HapticSample>& samples);
	void Play(const HapticEffect& e);
	void Update(float dt);
	void Play(HapticHandle h);
	void Pause(HapticHandle h);
	void Reset(HapticHandle h);
	void Create(HapticHandle h, std::unique_ptr<IPlayable> playable);
	const std::unique_ptr<SuitHardwareInterface>& Hardware();
private:
	std::vector<TimeInstant> _queuedFrames;
	std::vector<TimeInstant> _queuedSamples;
	std::vector<TimeInstant> _queuedEffects;
	std::unique_ptr<SuitHardwareInterface> _suit;
	std::unordered_map<Location, HapticQueue> _model;
	std::unordered_map<HapticHandle, std::unique_ptr<IPlayable>> _effects;
	void executePendingFrames(float dt);
	void executePendingSamples(float dt);
	void executePendingEffects(float dt);

	void updateLocationModels(float dt);
};

