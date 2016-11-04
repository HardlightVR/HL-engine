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

void HapticQueue::Put(unsigned int priority, HapticEvent effect)
{
	PriorityPair pair(priority, effect);
	if (_queue.size() == 0)
	{
		//std::cout << "Adding effect " << int(effect.Effect) << " to queue" << "\n";
		_queue.push_back(pair);
	} else if(effect.DurationType() != Duration::OneShot || isHigherPriorityOneShot(effect, _queue.at(0), priority))
	{
		auto iter = std::lower_bound(_queue.begin(), _queue.end(), pair, [](const PriorityPair& lhs, const PriorityPair& rhs) {
			return lhs.first < rhs.first;
		});
		//std::cout << "Adding effect " << int(effect.Effect) << " to queue" << "\n";

		_queue.insert(iter, pair);
	}
	else {
		//std::cout << "NOT ADDING " << int(effect.Effect) << " to queue" << "\n";

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
				effect.TimeElapsed += deltaTime;
			} else
			{
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
		return m.second.Dirty || m.first < firstPriority && m.second.DurationType() == ::Duration::OneShot;
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

HapticEvent* HapticQueue::GetNextEvent()
{
	Purge();
	if (_queue.size() > 0)
	{
		auto hapticEvent = _queue[0];
		if (hapticEvent.second.DurationType() == ::Duration::OneShot)
		{
			_queue[0].second.Dirty = true;
			//_queue.erase(_queue.begin());
		}
			return &_queue[0].second;
		
	}

	return nullptr;
}




bool HapticQueue::isHigherPriorityOneShot(const HapticEvent& b, const PriorityPair& a, unsigned int priority)
{
	return a.first < priority && b.DurationType() == Duration::OneShot;
}
