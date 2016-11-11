#include "stdafx.h"
#include "PlayablePattern.h"
#include "HapticsExecutor.h"

#include "PlayableSequence.h"
PlayablePattern::PlayablePattern(std::vector<HapticFrame> frames):_sourceOfTruth(frames), _paused(true)
{
	for (const auto& e : frames) {
		_effects.push_back(Instant<HapticFrame>(e, e.Time));
	}
}




PlayablePattern::~PlayablePattern()
{
}

void PlayablePattern::Play()
{
	_paused = false;
}

void PlayablePattern::Reset(PriorityModel & model)
{
	//use pause to remove anything that is playing in the Model
	this->Pause(model);
	//then clear all actives, reset everything from our source of truth
	_activeEffects.clear();
	_effects.clear();
	for (const auto& e : _sourceOfTruth) {
		_effects.push_back(Instant<HapticFrame>(e, e.Time));
	}
}

void PlayablePattern::Pause(PriorityModel & model)
{
}

void PlayablePattern::Update(float dt, PriorityModel & model, const std::unordered_map<std::string, Atom>&, HapticsExecutor& executor)
{
	if (_paused) { return; }

	for (auto& effect : _effects) {
		if (effect.Executed) {
			continue;
		}

		effect.Time += dt;
		if (effect.Expired()) {
			auto& h = effect.Item;
			effect.Executed = true;
			executor.Create(effect.Handle, std::unique_ptr<IPlayable>(new PlayableSequence(h.Frame, h.Area)));
			executor.Play(effect.Handle);

			//HapticEffect* h = static_cast<HapticEffect*>(effect.Item.get());
			//todo: Need the logic for playing in multiple spots
			//use real priority
			//use actual effect as well..

		}
	}
}

uint32_t PlayablePattern::GetHandle() const
{
	return _handle;
}
