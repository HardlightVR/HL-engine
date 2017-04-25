#include "StdAfx.h"
#include "HapticsPlayer.h"
#include "HapticEvent.h"
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include "PlayableEffect.h"
using namespace std;


HapticsPlayer::HapticsPlayer(std::shared_ptr<InstructionSet> iset):
	_model(), 
	_iset(iset),
	_paused(false),
	_generator(_model)
{
	
}

HapticsPlayer::~HapticsPlayer()
{
}

void HapticsPlayer::Play(HapticHandle hh)
{
	if (auto effect = toInternalUUID(hh)) {
		effect.get()->Play();
	}
}

void HapticsPlayer::Pause(HapticHandle hh)
{
	if (auto effect = toInternalUUID(hh)) {
		effect.get()->Pause();
	}
}

void HapticsPlayer::Restart(HapticHandle hh)
{
	if (auto effect = toInternalUUID(hh)) {
		NS::Playable::Restart(effect.get());
	}
}

void HapticsPlayer::Stop(HapticHandle hh)
{
	if (auto effect = toInternalUUID(hh)) {
		effect.get()->Stop();
	}
}


void HapticsPlayer::Release(HapticHandle hh)
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

void HapticsPlayer::Create(HapticHandle h, std::vector<TinyEffect> decoded)
{
	auto id = _uuidGen();

	_outsideHandleToUUID[h] = id;
	_effects[uuid_hasher(id)] = std::unique_ptr<IPlayable>(new PlayableEffect(decoded, _generator));
}

PriorityModel & HapticsPlayer::GetModel()
{
	return _model;
}





bool EffectIsExpired(const std::unique_ptr<IPlayable> &p, bool isGlobalPause) {
	const auto& effectInfo = p->GetInfo();
	return !effectInfo.Playing();

	/*
	if (isGlobalPause) {
		return effectInfo.CurrentTime() >= effectInfo.Duration();
		//return p->CurrentTime() >= p->GetTotalPlayTime();
	}
	else {
		return !effectInfo.Playing() || effectInfo.CurrentTime() >= effectInfo.Duration();
		return !p->IsPlaying() || p->CurrentTime() >= p->GetTotalPlayTime();
	}
	*/
}

std::vector<PriorityModel::ExecutionCommand> HapticsPlayer::Update(float dt)
{
	//updateLocationModels(dt);

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

	return _model.Update(dt);
}


void HapticsPlayer::PlayAll()
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

void HapticsPlayer::PauseAll()
{
	_paused = true;


	for (auto& effect : _effects) {
		if (effect.second->IsPlaying()) {
			effect.second->Pause();
			_frozenEffects.push_back(effect.first);
		}
	}
}

void HapticsPlayer::ClearAll()
{
	for (auto& effect : _effects) {
		effect.second->Pause();
	}
	_outsideHandleToUUID.clear();
	_effects.clear();
	_releasedEffects.clear();
}






boost::optional<const std::unique_ptr<IPlayable>&> HapticsPlayer::toInternalUUID(HapticHandle hh)
{
	auto h = uuid_hasher(_outsideHandleToUUID[hh]);
	if (_effects.find(h) != _effects.end()) {
		return _effects.at(h);
	}
	return boost::optional<const std::unique_ptr<IPlayable>&>();
}
