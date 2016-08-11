#pragma once
#include <vector>
#include "HapticArgs.h"
#include "HapticCache.h"
#include "Parser.h"
#include "HapticClasses.h"
#include <memory>
#include "Loader.h"
using namespace std;

template<class TArgType, class HapticArgs>
class IResolvable
{
public:
	virtual ~IResolvable()
	{
	}

	virtual std::vector<HapticArgs> Resolve(TArgType args) = 0;
};
class DependencyResolver
{
public:
	DependencyResolver(const std::string& basePath);
	~DependencyResolver();
	//void SetBasePath(const std::string& path);
	static Location ComputeLocationSide(JsonLocation loc, Side side);
	std::vector<HapticEffect> ResolveSequence(const std::string& name, Location location) const;

private:
	shared_ptr<IResolvable<SequenceArgs, HapticEffect>> _sequenceResolver;
	shared_ptr<IResolvable<PatternArgs, HapticFrame>> _patternResolver;
	shared_ptr<IResolvable<ExperienceArgs, HapticSample>> _experienceResolver;
	Loader _loader;
	
};




class SequenceResolver : public IResolvable<SequenceArgs, HapticEffect>
{
public:
	SequenceResolver(shared_ptr<SequenceLoader> loader);
	~SequenceResolver();
	vector<HapticEffect> Resolve(SequenceArgs args) override;
private:
	HapticCache<HapticEffect> _cache;
	shared_ptr<SequenceLoader> _sequenceLoader;
	static HapticEffect transformSequenceItemIntoEffect(const SequenceItem& seq, Location loc);
};

class PatternResolver : public IResolvable<PatternArgs, HapticFrame>
{
public:
	PatternResolver(shared_ptr<IResolvable<SequenceArgs, HapticEffect>>  seq, shared_ptr<PatternLoader>);
	~PatternResolver();
	vector<HapticFrame> Resolve(PatternArgs args) override;
private:
	HapticCache<HapticFrame> _cache;
	shared_ptr<PatternLoader> _patternLoader;
	shared_ptr<IResolvable<SequenceArgs, HapticEffect>>  _seqResolver;
	HapticFrame transformFrameToHapticFrame(const Frame& frame, Side side) const;
	static Side ComputeSidePrecedence(Side inputSide, Side programmaticSide);
};

class ExperienceResolver : public IResolvable<ExperienceArgs, HapticSample>
{
public:
	ExperienceResolver(shared_ptr<IResolvable<PatternArgs, HapticFrame>> pat, shared_ptr<ExperienceLoader> el);
	~ExperienceResolver();
	vector<HapticSample> Resolve(ExperienceArgs args) override;
private:
	HapticCache<HapticSample> _cache;
	shared_ptr<ExperienceLoader> _experienceLoader;
	shared_ptr<IResolvable<PatternArgs, HapticFrame>> _patResolver;
	HapticSample transformMomentToHapticSample(Moment moment, Side side) const;
};
