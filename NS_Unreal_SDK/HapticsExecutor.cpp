#include "StdAfx.h"
#include "HapticsExecutor.h"
#include "HapticEvent.h"
using namespace std;


HapticsExecutor::HapticsExecutor(std::unique_ptr<SuitHardwareInterface> s):_suit(std::move(s)), _model(_suit)
{

}

HapticsExecutor::~HapticsExecutor()
{
}
void HapticsExecutor::Create(HapticHandle h, std::unique_ptr<IPlayable> playable)
{
	_effects[h] = std::move(playable);
}

void HapticsExecutor::Play(HapticHandle h)
{
	if (_effects.find(h) != _effects.end()) {
		_effects.at(h)->Play();
	}
}

void HapticsExecutor::Pause(HapticHandle h)
{
	if (_effects.find(h) != _effects.end()) {
		_effects.at(h)->Pause(_model);
	}
}

void HapticsExecutor::Reset(HapticHandle h)
{
	std::cout << "resetting handle " << h << '\n';
	if (_effects.find(h) != _effects.end()) {
		_effects.at(h)->Reset(_model);
	}
}

void HapticsExecutor::Release(HapticHandle h)
{
	std::cout << "releasing handle" <<  h << '\n';
	auto it = _effects.find(h);
	_effects.erase(it);
}



void HapticsExecutor::Update(float dt)
{
	updateLocationModels(dt);
	std::for_each(_effects.begin(), _effects.end(), [&](std::pair<const HapticHandle, std::unique_ptr<IPlayable>>& p) {

		p.second->Update(dt, _model);
	});
	//executePendingSamples(dt);
	//executePendingFrames(dt);
	//executePendingEffects(dt);

}

const std::unique_ptr<SuitHardwareInterface>& HapticsExecutor::Hardware()
{
	return _suit;
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
