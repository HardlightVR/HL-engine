#include "stdafx.h"
#include "HapticQueue.h"
#include "HapticEvent.h"
#include <algorithm>
#include <iostream>
HapticQueue::HapticQueue():Dirty(false)
{
	_queue.reserve(16);
}


HapticQueue::~HapticQueue()
{
}

boost::optional<boost::uuids::uuid> HapticQueue::Put(unsigned int priority, HapticEvent effect)
{
	PriorityPair pair(priority, effect);
	assert(pair.second.Handle == effect.Handle);
	if (_queue.size() == 0)
	{
		//std::cout << "Adding effect " << int(effect.Effect) << " to queue" << "\n";
		_queue.push_back(pair);
		if (effect.DurationType() != Duration::OneShot) {
			return effect.Handle;

		}
		else {
			return boost::optional<boost::uuids::uuid>();
		}
	} else if(effect.DurationType() != Duration::OneShot || isHigherPriorityOneShot(effect, _queue.at(0), priority))
	{
		auto iter = std::lower_bound(_queue.begin(), _queue.end(), pair, [](const PriorityPair& lhs, const PriorityPair& rhs) {
			return lhs.first < rhs.first;
		});
		//std::cout << "Adding effect " << int(effect.Effect) << " to queue" << "\n";

		_queue.insert(iter, pair);
		return effect.Handle;
	}
	else {
		//std::cout << "NOT ADDING " << int(effect.Effect) << " to queue" << "\n";
		return boost::optional<boost::uuids::uuid>();
	}
}



void HapticQueue::Update(float deltaTime)
{
	for (auto& pair : _queue)
	{
		auto& effect = pair.second;
		if (effect.Dirty)
		{
			continue;
		}

		if (effect.DurationType() == Duration::Variable)
		{
			if (effect.TimeElapsed < effect.Duration - deltaTime)
			{
			//	std::cout << "Duration of effect is variable, so gonna add time\n";

				effect.TimeElapsed += deltaTime;
			} else
			{
				//std::cout << "duration of effect is variable, but time is all elapsed, so marking dirty\n";
				this->Dirty = effect.Dirty = true;
			}
		}
	}

}

struct shouldEffectBeRemoved
{
	const unsigned int firstPriority;
	shouldEffectBeRemoved(const unsigned int p) : firstPriority(p){}
	bool operator()(const PriorityPair& m) const
	{
		return (m.second.Dirty) || m.first < firstPriority && m.second.DurationType() == ::Duration::OneShot;
	}
};

void HapticQueue::Purge()
{
	if (_queue.size() == 0)
	{
		return;
	}
	_queue.erase(std::remove_if(_queue.begin(), _queue.end(), shouldEffectBeRemoved(_queue[0].first)), _queue.end());
}

void HapticQueue::Clear()
{
	_queue.clear();
	Dirty = false;
}

boost::optional<HapticEvent> HapticQueue::Remove(boost::uuids::uuid id)
{
	auto any = std::find_if(_queue.begin(), _queue.end(), [id](const PriorityPair& p) {return p.second.Handle == id; });
	if (any == _queue.end()) {
		return boost::optional<HapticEvent>();
	}
	else {
		HapticEvent whichExpired = (*any).second;
		this->Dirty = true;
		_queue.erase(std::remove_if(_queue.begin(), _queue.end(), [id](const PriorityPair& h) {return h.second.Handle == id; }));
		return whichExpired;
	}
}

HapticEvent* HapticQueue::GetNextEvent()
{
	Purge();
	if (_queue.size() > 0)
	{
		auto hapticEvent = _queue[0];
		if (hapticEvent.second.DurationType() == ::Duration::OneShot)
		{
			_queue[0].second.Dirty = true;
		}
		//temporary pointer to this event, will be invalid when the next purge happens.
		//only need for executing right then
			return &_queue[0].second;
		
	}

	return nullptr;
}




bool HapticQueue::isHigherPriorityOneShot(const HapticEvent& b, const PriorityPair& a, unsigned int priority)
{
	return a.first < priority && b.DurationType() == Duration::OneShot;
}
