#include "StdAfx.h"
#include "PlayableSequence.h"
#include "HapticEvent.h"
#include "HapticsExecutor.h"

PlayableSequence::PlayableSequence(std::vector<JsonSequenceAtom> j, Area loc):_effects(j), _paused(false), _location(loc)
{
	_liveEffects.clear();
	for (const auto& e : _effects) {
		_liveEffects.push_back(MyInstant(0.0, e));
	}
}

void PlayableSequence::Play()
{
	_paused = false;
}

void PlayableSequence::Pause()
{
	_paused = true;
}



void PlayableSequence::Update(float dt, std::unordered_map<Location, HapticQueue> & model)
{
	if (_paused) { return; }

	for (auto& effect : _liveEffects) {
		effect.Time += dt;
		if (effect.Expired()) {
			auto& h = effect.Item;
			
			//HapticEffect* h = static_cast<HapticEffect*>(effect.Item.get());
			//todo: Need the logic for playing in multiple spots
			//use real priority
			//use actual effect as well..
			model[Location(_location)].Put(1, HapticEvent(Effect::Strong_Click_100, h.Duration));
		}
	}
	auto iter = std::remove_if(_liveEffects.begin(), _liveEffects.end(), [](const MyInstant& t) {return t.Expired(); });
	_liveEffects.erase(iter, _liveEffects.end());
}

int PlayableSequence::GetHandle() const
{
	return _handle;
}


PlayableSequence::~PlayableSequence()
{
}
