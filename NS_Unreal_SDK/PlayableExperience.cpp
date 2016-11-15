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
	//use pause to remove anything that is playing in the Model
	this->Pause(model);
	//then clear all actives, reset everything from our source of truth
	_activeEffects.clear();
	_effects.clear();
	for (const auto& e : _sourceOfTruth) {
		_effects.push_back(Instant<HapticSample>(e, e.Time));
	}
	_currentTime = 0;
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
			effect.Handle = effect.Handle = boost::uuids::random_generator()();
			_exec.Create(effect.Handle, std::unique_ptr<IPlayable>(new PlayablePattern(h.Frames, _exec)));
			_exec.Play(effect.Handle);
			_activeEffects.push_back(effect.Handle);
			//HapticEffect* h = static_cast<HapticEffect*>(effect.Item.get());
			//todo: Need the logic for playing in multiple spots
			//use real priority
			//use actual effect as well..

		}
	}
}

uint32_t PlayableExperience::GetHandle() const
{
	return _handle;
}

float PlayableExperience::GetTotalPlayTime() const
{
	return ::GetTotalPlayTime(_sourceOfTruth);
}

float PlayableExperience::CurrentTime() const
{
	return _currentTime;
}

bool PlayableExperience::IsPlaying() const
{
	return !_paused;
}
