#pragma once
#include "IPlayable.h"
#include "TimeInstant.h"
class PlayablePattern : public IPlayable
{
public:
	PlayablePattern(std::vector<HapticFrame>);
	~PlayablePattern();
	void Play() override;
	void Reset(PriorityModel & model) override;
	void Pause(PriorityModel & model) override;
	void Update(float dt, PriorityModel & model, const std::unordered_map<std::string, Atom>&, HapticsExecutor& h) override;
	uint32_t GetHandle() const override;
	float GetTotalPlayTime() const override;
	float CurrentTime() const override;
private:
	std::vector<Instant<HapticFrame>> _effects;
	std::vector<HapticFrame> _sourceOfTruth;
	std::vector<boost::uuids::uuid> _activeEffects;
	bool _paused;
	int _handle;
	float _currentTime;
};

