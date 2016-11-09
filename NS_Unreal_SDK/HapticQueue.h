#pragma once
#include <vector>
#include <boost/optional/optional.hpp>

class HapticEvent;
typedef std::pair<unsigned int, HapticEvent> PriorityPair;
class HapticQueue
{
public:
	HapticQueue();
	~HapticQueue();
	bool Dirty;
	void Put(unsigned int priority, HapticEvent effect);
	void Update(float deltaTime);
	void Purge();
	void Clear();
	
	HapticEvent* GetNextEvent();
	
private:
	//todo: CHANGE PRIORITY MULTIPLICATION!!
	std::vector<PriorityPair> _queue;
	static bool isHigherPriorityOneShot(const HapticEvent& b, const PriorityPair& a, unsigned int priority);

};

