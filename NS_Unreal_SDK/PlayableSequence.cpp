#include "StdAfx.h"
#include "PlayableSequence.h"
#include "HapticEvent.h"
#include "HapticsExecutor.h"
#include <functional>
#include <algorithm>
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



#define START_BITMASK_SWITCH(x) \
	for (uint32_t bit = 1; x >= bit; bit *=2) if (x & bit) switch(AreaFlag(bit))


void PlayableSequence::insertIntoModel(std::unordered_map<Location, HapticQueue> & model, MyInstant& ef, Location loc) {
	if (boost::optional<boost::uuids::uuid> id =
		model[loc].Put(1, HapticEvent(Effect::Strong_Click_100, ef.Item.Duration))) {
		_activeEffects.push_back(id.get());
		ef.Handle = id.get();
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

			START_BITMASK_SWITCH(uint32_t(_location))
			{
				case AreaFlag::Forearm_Left:
					insertIntoModel(model, effect, Location::Forearm_Left);
					break;
				case AreaFlag::Upper_Arm_Left:
					insertIntoModel(model, effect, Location::Upper_Arm_Left);
					break;
				case AreaFlag::Shoulder_Left:
					insertIntoModel(model, effect, Location::Shoulder_Left);
					break;
				case AreaFlag::Back_Left:
					insertIntoModel(model, effect, Location::Upper_Back_Left);
					break;
				case AreaFlag::Chest_Left:
					insertIntoModel(model, effect, Location::Chest_Left);
					break;
				case AreaFlag::Upper_Ab_Left:
					insertIntoModel(model, effect, Location::Upper_Ab_Left);
					break;
				case AreaFlag::Mid_Ab_Left:
					insertIntoModel(model, effect, Location::Mid_Ab_Left);
					break;
				case AreaFlag::Lower_Ab_Left:
					insertIntoModel(model, effect, Location::Lower_Ab_Left);
					break;
				case AreaFlag::Forearm_Right:
					insertIntoModel(model, effect, Location::Forearm_Right);
					break;
				case AreaFlag::Upper_Arm_Right:
					insertIntoModel(model, effect, Location::Upper_Arm_Right);
					break;
				case AreaFlag::Shoulder_Right:
					insertIntoModel(model, effect, Location::Shoulder_Right);
					break;
				case AreaFlag::Back_Right:
					insertIntoModel(model, effect, Location::Upper_Back_Right);
					break;
				case AreaFlag::Chest_Right:
					insertIntoModel(model, effect, Location::Chest_Right);
					break;
				case AreaFlag::Upper_Ab_Right:
					insertIntoModel(model, effect, Location::Upper_Ab_Right);
					break;
				case AreaFlag::Mid_Ab_Right:
					insertIntoModel(model, effect, Location::Mid_Ab_Right);
					break;
				case AreaFlag::Lower_Ab_Right:
					insertIntoModel(model, effect, Location::Lower_Ab_Right);
					break;
				default:
					break;
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

int PlayableSequence::GetHandle() const
{
	return _handle;
}


PlayableSequence::~PlayableSequence()
{
}
