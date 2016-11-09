#include "StdAfx.h"
#include "HapticsExecutor.h"
#include "HapticEvent.h"
using namespace std;


HapticsExecutor::HapticsExecutor(std::unique_ptr<SuitHardwareInterface> s):_suit(std::move(s))
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
		_effects.at(h)->Pause();
	}
}

void HapticsExecutor::Play(const std::vector<HapticFrame>& frames)
{
	for (const auto& f : frames)
	{
		_queuedFrames.push_back(TimeInstant(0, std::make_unique<HapticFrame>(f)));
	}
}


void HapticsExecutor::Play(std::vector<HapticEffect> effects)
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

void HapticsExecutor::Play(const HapticEffect& e)
{
	_queuedEffects.push_back(TimeInstant(0, std::make_unique<HapticEffect>(e)));
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



void HapticsExecutor::executePendingFrames(float deltaTime)
{
	std::for_each(_queuedFrames.begin(), _queuedFrames.end(), [&](TimeInstant& t) {
		t.Time += deltaTime;
		if (t.Expired()) {
			HapticFrame* frame = static_cast<HapticFrame*>(t.Item.get());
			for (auto& sequence : frame->Frame)
			{
				for (auto& effect : sequence.Effects)
				{
					effect.Priority = frame->Priority;

					_queuedEffects.push_back(TimeInstant(0, move(std::make_unique<HapticEffect>(effect))));
				}
			}
		}
	});
	auto iter = std::remove_if(_queuedFrames.begin(), _queuedFrames.end(), [](const TimeInstant& t) {return t.Expired(); });
	_queuedFrames.erase(iter, _queuedFrames.end());
	
/*
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
	*/
}

void HapticsExecutor::executePendingSamples(float deltaTime)
{
	std::for_each(_queuedSamples.begin(), _queuedSamples.end(), [&](TimeInstant& t) {
		t.Time += deltaTime;
		//std::cout << "Instant time: " << t.Time << " Instant Expires: " << t.Item->GetTime() << '\n';
		//std::cout << "Sample is " << (t.Expired() ? "expired" : " NOT expired ") << '\n';
		if (t.Expired()) {
			HapticSample* e = static_cast<HapticSample*>(t.Item.get());
			
			for (auto& frame : e->Frames)
			{
				frame.Priority = e->Priority;
				_queuedFrames.push_back(TimeInstant(0, std::make_unique<HapticFrame>(std::move(frame))));

			}
		}
	});
	auto iter = std::remove_if(_queuedSamples.begin(), _queuedSamples.end(), [](const TimeInstant& t) {return t.Expired(); });
	_queuedSamples.erase(iter, _queuedSamples.end());
/*
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
	*/
}

void HapticsExecutor::executePendingEffects(float deltaTime)
{

	std::for_each(_queuedEffects.begin(), _queuedEffects.end(), [&](TimeInstant& t) {
		t.Time += deltaTime; 
		if (t.Expired()) {
			HapticEffect* e = static_cast<HapticEffect*>(t.Item.get());
			_model[e->Location].Put(e->Priority, HapticEvent(e->Effect, e->Duration));
		}
	});

	auto iter = std::remove_if(_queuedEffects.begin(), _queuedEffects.end(), [](const TimeInstant& t) {return t.Expired(); });
	_queuedEffects.erase(iter, _queuedEffects.end());

	/*
	auto iter = _queuedEffects.begin();
	while (iter != _queuedEffects.end())
	{
		iter->Time += deltaTime;
		if (iter->Expired())
		{
			//std::cout << "Expired at " << iter->Time << "\n";
			//take ownership
			std::unique_ptr<HapticEffect> e(static_cast<HapticEffect*>(iter->Item.release()));
			_model[e->Location].Put(e->Priority, HapticEvent(e->Effect, e->Duration));
			//iter->Item.release();
			iter = std::remove(_queuedEffects.begin(), _queuedEffects.end(), iter);
			//iter = _queuedEffects.erase(iter);
		} else
		{
			++iter;
		}

	}
	*/
}

void HapticsExecutor::updateLocationModels(float deltaTime)
{
	
	vector<pair<Duration, pair<Location, Effect>>> toExecute;
	for (auto& queue : _model)
	{
		_model[queue.first].Update(deltaTime);
		HapticEvent* effect = _model[queue.first].GetNextEvent();
		if (effect == nullptr)
		{
			if (queue.second.Dirty)
			{
				_suit->HaltEffect(queue.first);
				_model[queue.first].Dirty = false;
			}
			continue;
		}
		else {
			//BUG: Need to not send continuous play repeatedly 
			
			if (!effect->Sent) {
				effect->Sent = true;
				//effect->Sent = true;
				Duration d = Duration(effect->DurationType());

				//Duration d = Duration(Duration::OneShot);
				Effect e = Effect(effect->Effect);
				toExecute.push_back(pair<Duration, pair<Location, Effect>>(
					d,
					pair<Location, Effect>(
						queue.first,e
						)));
			}
				
			
		}
	
	}

	///if (toExecute.size() > 1 && toExecute.size() <= 4) {
		_suit->UseDeferredMode();
	//}
	//else {
//		_suit->UseImmediateMode();
	//}
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
	//	boost::this_thread::sleep(boost::posix_time::millisec(1));
	}

}
