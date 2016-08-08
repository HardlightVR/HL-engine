#pragma once
#include <unordered_map>
#include <unordered_set>
#include "HapticArgs.h"
template<class T>
class HapticCache
{
public:
	HapticCache();
	~HapticCache();
	void Cache(const HapticArgs& href, std::vector<T> data);
	std::vector<T> Get(const HapticArgs& href);
	bool Contains(const HapticArgs& href);
private:
	std::unordered_map<int, std::vector<T>> _resolvedHaptics;

};

