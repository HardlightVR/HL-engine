#include "StdAfx.h"
#include "PlayableSequence.h"
#include "HapticEvent.h"
#include "HapticsExecutor.h"
#include <functional>
#include <algorithm>
PlayableSequence::PlayableSequence(std::vector<JsonSequenceAtom> j, Area loc):_sourceOfTruth(j), _paused(true), _location(loc)
{
	//_liveEffects.clear();
	for (const auto& e : j) {
		_effects.push_back(MyInstant(0.0, e));
	}
}

void PlayableSequence::Play()
{
	_paused = false;
}

void PlayableSequence::Reset()
{
	_paused = true;
	_activeEffects.clear();
	_effects.clear();
	for (const auto& e : _sourceOfTruth) {
		_effects.push_back(MyInstant(0.0, e));
	}
}


void PlayableSequence::Pause(std::unordered_map<Location, HapticQueue> & model)
{
	_paused = true;
	auto iter = _activeEffects.begin();
	while (iter != _activeEffects.end()) {
		if (boost::optional<HapticEvent> pausedHaptic = model[Location(_location)].Remove(*iter)) {
			auto myHandleToThePausedHaptic = std::find_if(_effects.begin(), _effects.end(), [&](const MyInstant& m) {return m.Handle == *iter; });
			auto &realThing = *myHandleToThePausedHaptic;
			realThing.Time = 0;
			realThing.Executed = false;
			realThing.Item.Duration = realThing.Item.Duration - pausedHaptic->TimeElapsed;
			iter = _activeEffects.erase(iter);
		}
		else {
			++iter;
		}
	}
	

}



void PlayableSequence::Update(float dt, std::unordered_map<Location, HapticQueue> & model)
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
			//HapticEffect* h = static_cast<HapticEffect*>(effect.Item.get());
			//todo: Need the logic for playing in multiple spots
			//use real priority
			//use actual effect as well..
			if (boost::optional<boost::uuids::uuid> id = 
				model[Location(_location)].Put(1, HapticEvent(Effect::Strong_Click_100, h.Duration))) {
				_activeEffects.push_back(id.get());
				effect.Handle = id.get();
			}
		}
	}
	//auto iter = std::remove_if(_effects.begin(), _effects.end(), [](const MyInstant& t) {return t.Expired(); });
	//_effects.erase(iter, _liveEffects.end());
}

int PlayableSequence::GetHandle() const
{
	return _handle;
}


PlayableSequence::~PlayableSequence()
{
}
