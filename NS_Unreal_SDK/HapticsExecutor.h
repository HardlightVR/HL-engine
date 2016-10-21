#pragma once
#include "TimeInstant.h"
#include "SuitHardwareInterface.h"
#include <vector>
#include "HapticQueue.h"
#include "HapticClasses.h"

class HapticsExecutor
{
public:
	HapticsExecutor(std::shared_ptr<SuitHardwareInterface>);
	~HapticsExecutor();
	void Play(const std::vector<HapticFrame>& frames);
	void Play(std::vector<HapticEffect> effects);
	void Play(const std::vector<HapticSample>& samples);
	void Play(const HapticEffect& e);
	void Update(float dt);
private:
	std::vector<TimeInstant> _queuedFrames;
	std::vector<TimeInstant> _queuedSamples;
	std::vector<TimeInstant> _queuedEffects;
	std::shared_ptr<SuitHardwareInterface> _suit;
	std::unordered_map<Location, HapticQueue> _model;

	void executePendingFrames(float dt);
	void executePendingSamples(float dt);
	void executePendingEffects(float dt);

	void updateLocationModels(float dt);
};

