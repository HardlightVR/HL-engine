#include "stdafx.h"
#include "PlayableExperience.h"
#include <boost\uuid\uuid.hpp>
#include "HapticEvent.h"
#include "HapticsExecutor.h"
#include <functional>
#include <algorithm>
#include "PriorityModel.h"
#include "PlayablePattern.h"


PlayableExperience::PlayableExperience(std::vector<HapticSample> ex, HapticsExecutor& exec):_sourceOfTruth(ex), _paused(true), _exec(exec)
{
	for (const auto& e : ex) {
		_effects.push_back(Instant<HapticSample>(e, e.Time));
	}
}


PlayableExperience::~PlayableExperience()
{
}

void PlayableExperience::Play()
{
	_paused = false;
	for (auto& ef : _activeEffects) {
		_exec.Play(ef);
	}
}

void PlayableExperience::Reset(PriorityModel & model)
{
}

void PlayableExperience::Pause(PriorityModel & model)
{
	_paused = true;
	for (auto& ef : _activeEffects) {
	
		_exec.Pause(ef);
	}
}

void PlayableExperience::Update(float dt, PriorityModel & model, const std::unordered_map<std::string, Atom>&)
{
	if (_paused) { return; }
	_currentTime += dt;

	for (auto& effect : _effects) {
		if (effect.Executed) {
			continue;
		}

		effect.Time += dt;
		if (effect.Expired()) {
			auto& h = effect.Item;
			effect.Executed = true;

			_exec.Create(effect.Handle, std::unique_ptr<IPlayable>(new PlayablePattern(h.Frames, _exec)));
			_exec.Play(effect.Handle);

			//HapticEffect* h = static_cast<HapticEffect*>(effect.Item.get());
			//todo: Need the logic for playing in multiple spots
			//use real priority
			//use actual effect as well..

		}
	}
}

uint32_t PlayableExperience::GetHandle() const
{
	return uint32_t();
}

float PlayableExperience::GetTotalPlayTime() const
{
	return 0.0f;
}

float PlayableExperience::CurrentTime() const
{
	return 0.0f;
}

bool PlayableExperience::IsPlaying() const
{
	return false;
}
