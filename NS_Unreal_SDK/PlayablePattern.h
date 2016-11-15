#pragma once
#include "IPlayable.h"
#include "TimeInstant.h"
class PlayablePattern : public IPlayable
{
public:
	PlayablePattern(std::vector<HapticFrame>, HapticsExecutor& _exec);
	~PlayablePattern();
	void Play() override;
	void Reset(PriorityModel & model) override;
	void Pause(PriorityModel & model) override;
	void Update(float dt, PriorityModel & model, const std::unordered_map<std::string, Atom>&) override;
	uint32_t GetHandle() const override;
	float GetTotalPlayTime() const override;
	float CurrentTime() const override;
	bool IsPlaying() const override;
private:
	std::vector<Instant<HapticFrame>> _effects;
	std::vector<HapticFrame> _sourceOfTruth;
	std::vector<boost::uuids::uuid> _activeEffects;
	HapticsExecutor& _exec;

	bool _paused;
	int _handle;
	float _currentTime;
};

