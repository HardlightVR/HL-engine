#include "stdafx.h"
#include "PlayablePattern.h"
#include "HapticsExecutor.h"

#include "PlayableSequence.h"
PlayablePattern::PlayablePattern(std::vector<HapticFrame> frames, HapticsExecutor& exec):_sourceOfTruth(frames), _paused(true), _exec(exec)
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
	//If the effect was paused, we need to resume the sequences that were paused. 
	if (_paused) {
		_paused = false;
		for (auto& effect : _activeEffects) {
				_exec.Play(effect);
		}
	}


	//Additionally, if they called Play() after the effect was expired, Play will 
	//now start playing from the beginning immediately
	if (_currentTime >= GetTotalPlayTime()) {
		_activeEffects.clear();
		_effects.clear();
		for (const auto& e : _sourceOfTruth) {
			_effects.push_back(Instant<HapticFrame>(e, e.Time));
		}
		_currentTime = 0;
	}
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
	_currentTime = 0;
}

void PlayablePattern::Pause(PriorityModel & model)
{
	_paused = true;
	for (auto& effect : _activeEffects) {
		_exec.Pause(effect);
	}
}

void PlayablePattern::Update(float dt, PriorityModel & model, const std::unordered_map<std::string, Atom>&)
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
			effect.Handle = boost::uuids::random_generator()();
			_exec.Create(effect.Handle, std::unique_ptr<IPlayable>(new PlayableSequence(h.Frame, h.Area)));
			_exec.Play(effect.Handle);
			_activeEffects.push_back(effect.Handle);

			//HapticEffect* h = static_cast<HapticEffect*>(effect.Item.get());
			//use real priority
	

		}
	}
}

uint32_t PlayablePattern::GetHandle() const
{
	return _handle;
}

float PlayablePattern::GetTotalPlayTime() const
{
	return ::GetTotalPlayTime(_sourceOfTruth);
}

float PlayablePattern::CurrentTime() const
{
	return _currentTime;
}

bool PlayablePattern::IsPlaying() const
{
	return !_paused;
}

