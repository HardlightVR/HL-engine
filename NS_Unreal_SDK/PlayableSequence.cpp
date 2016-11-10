#include "StdAfx.h"
#include "PlayableSequence.h"
#include "HapticEvent.h"
#include "HapticsExecutor.h"
#include <functional>
#include <algorithm>
#include "PriorityModel.h"
PlayableSequence::PlayableSequence(std::vector<JsonSequenceAtom> j, AreaFlag loc):_sourceOfTruth(j), _paused(true), _location(loc)
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

void PlayableSequence::Reset(PriorityModel &model)
{
	//use pause to remove anything that is playing in the Model
	this->Pause(model);
	//then clear all actives, reset everything from our source of truth
	_activeEffects.clear();
	_effects.clear();
	for (const auto& e : _sourceOfTruth) {
		_effects.push_back(MyInstant(0.0, e));
	}
}



#define START_BITMASK_SWITCH(x) \
	for (uint32_t bit = 1; x >= bit; bit *=2) if (x & bit) switch(AreaFlag(bit))


void PlayableSequence::Pause(PriorityModel &model)
{
	_paused = true;
	for (auto& ef : _effects) {
		if (boost::optional<HapticEvent> ev = model.Remove(_location, ef.Handle)) {
			auto pausedHaptic = std::find_if(_effects.begin(), _effects.end(), [&](const MyInstant& m) {return m.Handle == ev.get().Handle; });
			(*pausedHaptic).Time = 0;
			(*pausedHaptic).Executed = false;
			(*pausedHaptic).Item.Duration = (*pausedHaptic).Item.Duration - ev.get().TimeElapsed;
		}
		
	}
	


}





void PlayableSequence::Update(float dt, PriorityModel & model)
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
			auto ef = HapticEvent(Effect::Strong_Click_100, h.Duration);
			if (boost::optional<boost::uuids::uuid> id = model.Put(_location, ef)) {
				_activeEffects.push_back(id.get());
				effect.Handle = id.get();
			}
			//HapticEffect* h = static_cast<HapticEffect*>(effect.Item.get());
			//todo: Need the logic for playing in multiple spots
			//use real priority
			//use actual effect as well..
			
		}
	}
	//auto iter = std::remove_if(_effects.begin(), _effects.end(), [](const MyInstant& t) {return t.Expired(); });
	//_effects.erase(iter, _liveEffects.end());
}

uint32_t PlayableSequence::GetHandle() const
{
	return _handle;
}


PlayableSequence::~PlayableSequence()
{

}
