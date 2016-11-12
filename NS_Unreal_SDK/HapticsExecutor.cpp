#include "StdAfx.h"
#include "HapticsExecutor.h"
#include "HapticEvent.h"
using namespace std;


HapticsExecutor::HapticsExecutor(std::shared_ptr<InstructionSet> iset, std::unique_ptr<SuitHardwareInterface> s):_suit(std::move(s)), _model(_suit), _iset(iset)
{
	
}

HapticsExecutor::~HapticsExecutor()
{
}
void HapticsExecutor::Create(HapticHandle h, std::unique_ptr<IPlayable> playable)
{
	auto id = boost::uuids::random_generator()();
	_outsideHandleToUUID[h] = id;
	_effects[uuid_hasher(id)] = std::move(playable);
}

void HapticsExecutor::Play(HapticHandle hh)
{
	auto h = _outsideHandleToUUID[hh];
	if (_effects.find(uuid_hasher(h)) != _effects.end()) {
		_effects.at(uuid_hasher(h))->Play();
	}
}

void HapticsExecutor::Pause(HapticHandle hh)
{
	auto h = _outsideHandleToUUID[hh];

	if (_effects.find(uuid_hasher(h)) != _effects.end()) {
		_effects.at(uuid_hasher(h))->Pause(_model);
	}
}

void HapticsExecutor::Reset(HapticHandle hh)
{
	auto h = _outsideHandleToUUID[hh];

	if (_effects.find(uuid_hasher(h)) != _effects.end()) {
		_effects.at(uuid_hasher(h))->Reset(_model);
	}
}

void HapticsExecutor::Release(HapticHandle hh)
{
	auto h = _outsideHandleToUUID[hh];
	
	auto it = _effects.find(uuid_hasher(h));
	if (it != _effects.end()) {
		_garbageCan.push_back(h);
	}
	else {
		std::cout << "Tried to release a handle that I never had in the first place\n";
	}

	_outsideHandleToUUID.erase(_outsideHandleToUUID.find(hh));
}



void HapticsExecutor::Update(float dt)
{
	updateLocationModels(dt);
	std::for_each(_effects.begin(), _effects.end(), [&](std::pair<const HapticHandle, std::unique_ptr<IPlayable>>& p) {

		p.second->Update(dt, _model, _iset->Atoms(), *this);
	});

	_garbageCan.erase(remove_if(_garbageCan.begin(), _garbageCan.end(), [&](const auto& id) {

		return _effects.at(uuid_hasher(id))->CurrentTime() >= _effects.at(uuid_hasher(id))->GetTotalPlayTime();
	}));
	
	
	_effects.erase(remove_if(_effects.begin(), _effects.end(),       [&](const auto& p) {
	
		return p.second->CurrentTime() >= p.second->GetTotalPlayTime();
	}));
	

}

const std::unique_ptr<SuitHardwareInterface>& HapticsExecutor::Hardware()
{
	return _suit;
}

void HapticsExecutor::Play(boost::uuids::uuid h)
{
	if (_effects.find(uuid_hasher(h)) != _effects.end()) {
		_effects.at(uuid_hasher(h))->Play();
	}
}

void HapticsExecutor::Pause(boost::uuids::uuid h)
{

	if (_effects.find(uuid_hasher(h)) != _effects.end()) {
		_effects.at(uuid_hasher(h))->Pause(_model);
	}
}



void HapticsExecutor::Reset(boost::uuids::uuid h)
{
	if (_effects.find(uuid_hasher(h)) != _effects.end()) {
		_effects.at(uuid_hasher(h))->Reset(_model);
	}
}

void HapticsExecutor::Release(boost::uuids::uuid h)
{
	auto it = _effects.find(uuid_hasher(h));
	_effects.erase(it);
}

void HapticsExecutor::Create(boost::uuids::uuid id, std::unique_ptr<IPlayable> playable)
{
	_effects[uuid_hasher(id)] = std::move(playable);
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
