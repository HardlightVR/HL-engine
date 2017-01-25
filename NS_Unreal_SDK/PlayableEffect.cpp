#include "PlayableEffect.h"
#include "Locator.h"

#include <iostream>
#include "PriorityModel.h"
#include <iterator>
PlayableEffect::PlayableEffect(std::vector<TinyEffect> effects, HapticEventGenerator& gen):_effects(effects), _paused(true), _gen(gen)
{
	assert(!_effects.empty());
	_lastExecutedEffect = _effects.begin();
}


PlayableEffect::~PlayableEffect()
{
}

void PlayableEffect::Play()
{
	_paused = false;
	for (const auto& id : _activeEffects) {
		_gen.ResumeEvent(id);
	}
}

void PlayableEffect::Reset()
{
	_time = 0;
	_paused = true;
	_lastExecutedEffect = _effects.begin();
	for (const auto& id : _activeEffects) {
		_gen.RemoveEvent(id);
	}
	_activeEffects.clear();

}

void PlayableEffect::Pause()
{
	_paused = true;
	for (const auto& id : _activeEffects) {
		_gen.PauseEvent(id);
	}
	
}

void PlayableEffect::Update(float dt, const std::unordered_map<std::string, Atom>& atoms)
{
	if (_paused) {
		return;
	}

	_time += dt;
	
	
	std::vector<TinyEffect>::iterator current(_lastExecutedEffect);
	while (current != _effects.end()) {

		if (current->Time <= _time) {
			auto a = atoms.at(Locator::getTranslator().ToString(current->Effect)).GetEffect(current->Strength);
			if (auto optionalId = _gen.NewEvent((AreaFlag)current->Area, current->Duration, a)) {
				_activeEffects.push_back(optionalId.get());
			}
			
			
			std::advance(current, 1);
			_lastExecutedEffect = current;


		}
		else {
			_lastExecutedEffect = current;
			break;
		}
	}


} 

uint32_t PlayableEffect::GetHandle() const
{
	return uint32_t();
}

float PlayableEffect::GetTotalPlayTime() const
{
	return ::GetTotalPlayTime(_effects);
}

float PlayableEffect::CurrentTime() const
{
	return _time;
}

bool PlayableEffect::IsPlaying() const
{
	return !_paused;
}

void PlayableEffect::Release() const
{
}

void PlayableEffect::PlayFromStart() 
{
	//this->Reset(;
	this->Play();
}
