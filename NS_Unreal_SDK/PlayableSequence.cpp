#include "StdAfx.h"
#include "PlayableSequence.h"
#include "HapticEvent.h"
#include "HapticsExecutor.h"
#include <functional>
#include <algorithm>
#include "PriorityModel.h"
PlayableSequence::PlayableSequence(std::vector<JsonSequenceAtom> j, AreaFlag loc, float strength):_sourceOfTruth(j), _paused(true), _location(loc), _strength(strength)
{
	for (const auto& e : j) {
		_effects.push_back(Instant<JsonSequenceAtom>(e, e.Time));
	}
}

void PlayableSequence::Play()
{
	//If the user paused the effect, we want to resume where they left off.
	_paused = false;

	//Additionally, if they called Play() after the effect was expired, Play will 
	//now start playing from the beginning immediately
	if (_currentTime >= GetTotalPlayTime()) {
		_activeEffects.clear();
		_effects.clear();
		for (const auto& e : _sourceOfTruth) {
			_effects.push_back(Instant<JsonSequenceAtom>(e, e.Time));
		}
		_currentTime = 0;
	}
}

void PlayableSequence::Reset(PriorityModel &model)
{
	//use pause to remove anything that is playing in the Model
	this->Pause(model);
	//then clear all actives, reset everything from our source of truth
	_activeEffects.clear();
	_effects.clear();
	for (const auto& e : _sourceOfTruth) {
		_effects.push_back(Instant<JsonSequenceAtom>(e, e.Time));
	}
	_currentTime = 0;
}



#define START_BITMASK_SWITCH(x) \
	for (uint32_t bit = 1; x >= bit; bit *=2) if (x & bit) switch(AreaFlag(bit))


void PlayableSequence::Pause(PriorityModel &model)
{
	_paused = true;
	for (auto& ef : _effects) {
		if (boost::optional<HapticEvent> ev = model.Remove(_location, ef.Handle)) {
			auto pausedHaptic = std::find_if(_effects.begin(), _effects.end(), [&](const Instant<JsonSequenceAtom>& m) {return m.Handle == ev.get().Handle; });
			(*pausedHaptic).Time = 0;
			(*pausedHaptic).Executed = false;

			(*pausedHaptic).Item.Duration = (*pausedHaptic).Item.Duration - ev.get().TimeElapsed;
			//std::cout << "Pausing effect. Timeelapsed was " << ev.get().TimeElapsed << " and new duration is " << (*pausedHaptic).Item.Duration << '\n';

		}
		
	}
	


}





void PlayableSequence::Update(float dt, PriorityModel & model,const std::unordered_map<std::string, Atom>& atoms)
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
			if (atoms.find(h.Effect) != atoms.end()) {
				//here is where the natural strength is modulated by the overriding sequence strength
				auto ef = HapticEvent(atoms.at(h.Effect).GetEffect(h.Strength * _strength), h.Duration);
				if (boost::optional<boost::uuids::uuid> id = model.Put(_location, ef)) {
					_activeEffects.push_back(id.get());
					effect.Handle = id.get();
				}
			}
			else {
				std::cout << "Couldn't find an effect with that name\n";
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

float PlayableSequence::GetTotalPlayTime() const
{
	return ::GetTotalPlayTime(_sourceOfTruth);
}

float PlayableSequence::CurrentTime() const
{
	return _currentTime;
}

bool PlayableSequence::IsPlaying() const
{
	return !_paused;
}



PlayableSequence::~PlayableSequence()
{

}
