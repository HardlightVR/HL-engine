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


template <class T>
HapticCache<T>::HapticCache()
{
}

template <class T>
HapticCache<T>::~HapticCache()
{
}

template <class T>
void HapticCache<T>::Cache(const HapticArgs& href, std::vector<T> data)
{
	_resolvedHaptics[href.GetCombinedHash()] = data;
}

template <class T>
std::vector<T> HapticCache<T>::Get(const HapticArgs& href)
{
	if (_resolvedHaptics.find(href.GetCombinedHash()) != _resolvedHaptics.end())
	{
		//return _resolvedHaptics[href.GetCombinedHash()];
	}

	return std::vector<T>();
}

template <class T>
bool HapticCache<T>::Contains(const HapticArgs& href)
{
	return _resolvedHaptics.find(href.GetCombinedHash()) != _resolvedHaptics.end();
}
