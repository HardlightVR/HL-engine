#include "PlayableEffect.h"
#include "Locator.h"

#include <iostream>
#include "PriorityModel.h"
#include <iterator>

namespace NS {
	namespace Playable {
		void Restart(const std::unique_ptr<IPlayable>& playable) {
			playable->Stop();
			playable->Play();
		}
	}
}
PlayableEffect::PlayableEffect(std::vector<TinyEffect> effects, HapticEventGenerator& gen) :_effects(effects), _state(PlaybackState::IDLE), _gen(gen),
_id(boost::uuids::random_generator()())
{
	assert(!_effects.empty());
	reset();
}


PlayableEffect::~PlayableEffect()
{
}

void PlayableEffect::Play()
{
	switch (_state) {
	case PlaybackState::IDLE:
	//	reset();
		_state = PlaybackState::PLAYING;
		break;
	case PlaybackState::PAUSED:
		resume();
		_state = PlaybackState::PLAYING;
		break;
	case PlaybackState::PLAYING:
		//remain in playing state
		break;
	default:
		break;
	}
	
}

void PlayableEffect::Stop()
{

	switch (_state) {
		case PlaybackState::IDLE:
			//remain in idle state
			break;
		case PlaybackState::PAUSED:
			reset();
			_state = PlaybackState::IDLE;
			break;
		case PlaybackState::PLAYING:
			reset();
			_state = PlaybackState::IDLE;
			break;
		default:
			break;
	}
}

void PlayableEffect::Pause()
{
	switch (_state) {
	case PlaybackState::IDLE:
		//remain in idle state
		break;
	case PlaybackState::PAUSED:
		//remain in paused state
		break;
	case PlaybackState::PLAYING:
		pause();
		_state = PlaybackState::PAUSED;
		break;
	default:
		break;
	}
	
	
}


void PlayableEffect::Update(float dt, const std::unordered_map<std::string, Atom>& atoms)
{
	if (_state == PlaybackState::IDLE || _state == PlaybackState::PAUSED) {
		return;
	}

	_time += dt;
	
	std::vector<TinyEffect>::iterator current(_lastExecutedEffect);
	while (current != _effects.end()) {

		if (current->Time <= _time) {
			auto a = atoms.at(Locator::getTranslator().ToString(current->Effect)).GetEffect(current->Strength);
			_gen.NewEvent((AreaFlag)current->Area, current->Duration, a, _id);
			
			
			std::advance(current, 1);
			_lastExecutedEffect = current;


		}
		else {
			_lastExecutedEffect = current;
			break;
		}
	}

	if (_time >= GetTotalPlayTime()) {
		Stop();
	}

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
	return _state == PlaybackState::PLAYING;
}

PlayableInfo PlayableEffect::GetInfo() const
{
	return PlayableInfo(::GetTotalPlayTime(_effects), _time, _state == PlaybackState::PLAYING);
}

void PlayableEffect::Release()
{
	_gen.Remove(_id);
}

void PlayableEffect::reset()
{
	_time = 0;
	_lastExecutedEffect = _effects.begin();
	_gen.Remove(_id);
}

void PlayableEffect::pause()
{
	_gen.Pause(_id);
}

void PlayableEffect::resume() {
	_gen.Resume(_id);
}



