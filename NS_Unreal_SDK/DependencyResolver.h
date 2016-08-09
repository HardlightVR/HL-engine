#pragma once
#include <vector>
#include "HapticArgs.h"
#include "HapticCache.h"
#include "Parser.h"
#include "HapticClasses.h"

using namespace std;

class DependencyResolver
{
public:
	DependencyResolver();
	~DependencyResolver();
};


template<class TArgType, class HapticArgs>
class IResolvable
{
public:
	virtual ~IResolvable()
	{
	}

	virtual std::vector<HapticArgs> Resolve(TArgType args) = 0;
};


class SequenceResolver : public IResolvable<SequenceArgs, HapticEffect>
{
public:
	SequenceResolver(unordered_map<string, vector<SequenceItem>> loadedFiles);
	~SequenceResolver();
	vector<HapticEffect> Resolve(SequenceArgs args) override;
private:
	HapticCache<HapticEffect> _cache;
	unordered_map<string, vector<SequenceItem>> _loadedFiles;
	static HapticEffect transformSequenceItemIntoEffect(SequenceItem seq, Location loc);
};