#include "StdAfx.h"
#include "HapticsExecutor.h"
#include "HapticEvent.h"
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include "PlayableEffect.h"
using namespace std;


HapticsExecutor::HapticsExecutor(std::shared_ptr<InstructionSet> iset, std::unique_ptr<SuitHardwareInterface> s):
	_suit(std::move(s)), 
	_model(_suit), 
	_iset(iset),
	_paused(false),
	_generator(_model)
{
	
}

HapticsExecutor::~HapticsExecutor()
{
}

void HapticsExecutor::Play(HapticHandle hh)
{
	if (auto effect = findEffect(hh)) {
		effect.get()->Play();
	}
}

void HapticsExecutor::Pause(HapticHandle hh)
{
	if (auto effect = findEffect(hh)) {
		effect.get()->Pause();
	}
}

void HapticsExecutor::Restart(HapticHandle hh)
{
	if (auto effect = findEffect(hh)) {
		effect.get()->Restart();
	}
}

void HapticsExecutor::Stop(HapticHandle hh)
{
	if (auto effect = findEffect(hh)) {
		effect.get()->Stop();
	}
}


void HapticsExecutor::Release(HapticHandle hh)
{
	//std::cout << "Got a new handle to release\n";

	auto h = _outsideHandleToUUID[hh];
	
	auto it = _effects.find(uuid_hasher(h));
	if (it != _effects.end()) {
		_releasedEffects.push_back(Released(h));
	}
	else {
		//std::cout << "Tried to release a handle that I never had in the first place\n";
	}

	_outsideHandleToUUID.erase(_outsideHandleToUUID.find(hh));
}

void HapticsExecutor::Create(HapticHandle h, std::vector<TinyEffect> decoded)
{
	auto id = _uuidGen();

	_outsideHandleToUUID[h] = id;
	_effects[uuid_hasher(id)] = std::unique_ptr<IPlayable>(new PlayableEffect(decoded, _generator));
}



bool EffectIsExpired(const std::unique_ptr<IPlayable> &p, bool isGlobalPause) {

	if (isGlobalPause) {
		return p->CurrentTime() >= p->GetTotalPlayTime();
	}
	else {
		return !p->IsPlaying() || p->CurrentTime() >= p->GetTotalPlayTime();
	}
}

void HapticsExecutor::Update(float dt)
{
	updateLocationModels(dt);

	for (auto& effect : _effects) {
		effect.second->Update(dt, _iset->Atoms());
	}
	
	//mark & erase from _effects

	for (auto& released : _releasedEffects) {
		if (EffectIsExpired(_effects.at(uuid_hasher(released.ID)), _paused)) {
			released.NeedsSweep = true;
			_effects.at(uuid_hasher(released.ID))->Release();
			_effects.erase(uuid_hasher(released.ID));
		//	std::cout << "Hey, found an expired released handle, deleting from effects\n";
		}
	}

	//sweep from _released

	auto toRemove = std::remove_if(_releasedEffects.begin(), _releasedEffects.end(), [](const Released& e) {
		return e.NeedsSweep;
	});
	_releasedEffects.erase(toRemove, _releasedEffects.end());

	

}

const std::unique_ptr<SuitHardwareInterface>& HapticsExecutor::Hardware()
{
	return _suit;
}

void HapticsExecutor::PlayAll()
{
	_paused = false;

	
	//take anything that was frozen (not simply paused by the user) and resume it
	for (auto& effect : _effects) {
		if (std::find(_frozenEffects.begin(), _frozenEffects.end(), effect.first) != _frozenEffects.end()) {
			effect.second->Play();
		}
	}

	_frozenEffects.clear();
}
//frozen vs paused

void HapticsExecutor::PauseAll()
{
	_paused = true;


	for (auto& effect : _effects) {
		if (effect.second->IsPlaying()) {
			effect.second->Pause();
			_frozenEffects.push_back(effect.first);
		}
	}
}

void HapticsExecutor::ClearAll()
{
	for (auto& effect : _effects) {
		effect.second->Pause();
	}
	_outsideHandleToUUID.clear();
	_effects.clear();
	_releasedEffects.clear();
}






void HapticsExecutor::updateLocationModels(float deltaTime)
{
	
	

	_suit->UseDeferredMode();

	auto commands = _model.Update(deltaTime);
	
	for (auto& command : commands)
	{
		switch (command.Command) {
		case PriorityModel::Command::HALT:
			_suit->HaltEffect(command.Location);
			break;
		case PriorityModel::Command::PLAY:
			switch (command.Duration) {
			case Duration::Infinite:
			case Duration::Variable:
				_suit->PlayEffectContinuous(command.Location, command.Effect);
				break;
			case Duration::OneShot:
				_suit->HaltEffect(command.Location);
				_suit->PlayEffect(command.Location, command.Effect);
				_model.Clean(command.Location);
				break;
			default:
				break;
			}
		}
	}

}

boost::optional<const std::unique_ptr<IPlayable>&> HapticsExecutor::findEffect(HapticHandle hh)
{
	auto h = uuid_hasher(_outsideHandleToUUID[hh]);
	if (_effects.find(h) != _effects.end()) {
		return _effects.at(h);
	}
	return boost::optional<const std::unique_ptr<IPlayable>&>();
}
