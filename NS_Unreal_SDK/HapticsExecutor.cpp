#include "HapticsExecutor.h"
#include "HapticEvent.h"

using namespace std;


HapticsExecutor::HapticsExecutor(std::shared_ptr<SuitHardwareInterface> s):_suit(s)
{
}

HapticsExecutor::~HapticsExecutor()
{
}

void HapticsExecutor::Play(const std::vector<HapticFrame>& frames)
{
	for (auto& f : frames)
	{
		_queuedFrames.push_back(TimeInstant(0, std::move(std::make_unique<HapticFrame>(f))));
	}
}


void HapticsExecutor::Play(const std::vector<HapticEffect>& effects)
{
	for (auto& e : effects)
	{
		_queuedEffects.push_back(TimeInstant(0, move(std::make_unique<HapticEffect>(e))));
	}
}

void HapticsExecutor::Play(const std::vector<HapticSample>& samples)
{
	for (auto& s : samples)
	{
		_queuedSamples.push_back(TimeInstant(0, move(std::make_unique<HapticSample>(s))));
	}
}

void HapticsExecutor::Update(float dt)
{
	updateLocationModels(dt);
	executePendingSamples(dt);
	executePendingFrames(dt);
	executePendingEffects(dt);

}

void HapticsExecutor::executePendingFrames(float deltaTime)
{
	
	

	auto iter = _queuedFrames.begin();
	while (iter != _queuedFrames.end())
	{
		iter->Time += deltaTime;
		if (iter->Expired())
		{
			//take ownership
			std::unique_ptr<HapticFrame> frame(static_cast<HapticFrame*>(iter->Item.release()));
			for (auto& sequence : frame->Frame)
			{
				for (auto& effect : sequence.Effects)
				{
					effect.Priority = frame->Priority;
					
					_queuedEffects.push_back(TimeInstant(0, move(std::make_unique<HapticEffect>(effect))));
				}
			}
			iter = _queuedFrames.erase(iter);
			

			
		}
		else {
			++iter;
		}
	}
}

void HapticsExecutor::executePendingSamples(float deltaTime)
{
	
	auto iter = _queuedSamples.begin();
	while (iter != _queuedSamples.end())
	{
		iter->Time += deltaTime;
		if (iter->Expired())
		{
			//take ownership
			std::unique_ptr<HapticSample> e(static_cast<HapticSample*>(iter->Item.release()));
			for (auto& frame : e->Frames)
			{
				frame.Priority = e->Priority;
				_queuedFrames.push_back(TimeInstant(0, std::make_unique<HapticFrame>(frame)));

			}
			iter = _queuedSamples.erase(iter);
		} else
		{
			++iter;
		}
	}
}

void HapticsExecutor::executePendingEffects(float deltaTime)
{
	
	auto iter = _queuedEffects.begin();
	while (iter != _queuedEffects.end())
	{
		iter->Time += deltaTime;
		if (iter->Expired())
		{
			//take ownership
			std::unique_ptr<HapticEffect> e(static_cast<HapticEffect*>(iter->Item.release()));
			_model[e->Location].Put(e->Priority, HapticEvent(e->Effect, e->Duration));
			iter = _queuedEffects.erase(iter);
		} else
		{
			++iter;
		}

	}
}

void HapticsExecutor::updateLocationModels(float deltaTime)
{
	
	vector<pair<Duration, pair<Location, Effect>>> toExecute;
	for (auto& queue : _model)
	{
		_model[queue.first].Update(deltaTime);
		auto effect = _model[queue.first].GetNextEvent();
		if (!effect)
		{
			if (queue.second.Dirty)
			{
				_suit->HaltEffect(queue.first);
				_model[queue.first].Dirty = false;
			}
			continue;
		}
		auto e = effect.get();
		toExecute.push_back(pair<Duration, pair<Location, Effect>>(
			e.DurationType(), 
				pair<Location, Effect>(
			queue.first, e.Effect
		)));

		for (auto& pair : toExecute)
		{
			auto& hapticEvent = pair.second;
			switch (pair.first)
			{
				//fallthrough
			case Duration::Infinite:
			case Duration::Variable:
				_suit->PlayEffectContinuous(hapticEvent.first, hapticEvent.second);
				break;
			case Duration::OneShot:
				_suit->HaltEffect(hapticEvent.first);
				_suit->PlayEffect(hapticEvent.first, hapticEvent.second);
				_model[hapticEvent.first].Dirty = false;
				break;
			default:
				break;
			}
		}
	}
}
