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
	static Location ComputeLocationSide(JsonLocation loc, Side side);
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
	static HapticEffect transformSequenceItemIntoEffect(const SequenceItem& seq, Location loc);
};

class PatternResolver : public IResolvable<PatternArgs, HapticFrame>
{
public:
	PatternResolver(unordered_map<string, vector<Frame>> loadedFiles, std::unique_ptr<IResolvable<PatternArgs, HapticFrame>> seq);
	~PatternResolver();
	vector<HapticFrame> Resolve(PatternArgs args) override;
private:
	HapticCache<HapticFrame> _cache;
	unordered_map<string, vector<Frame>> _loadedFiles;
	std::unique_ptr<IResolvable<SequenceArgs, HapticEffect>> _seqResolver;
	static HapticFrame transformFrameToHapticFrame(const Frame& frame, Side side);
	static Side ComputeSidePrecedence(Side inputSide, Side programmaticSide);
};