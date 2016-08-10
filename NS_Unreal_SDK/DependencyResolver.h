#pragma once
#include <vector>
#include "HapticArgs.h"
#include "HapticCache.h"
#include "Parser.h"
#include "HapticClasses.h"
#include <memory>
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
	void SetBasePath(const std::string& path);
	static Location ComputeLocationSide(JsonLocation loc, Side side);
private:
	std::unique_ptr<IResolvable<SequenceArgs, HapticEffect>> _sequenceResolver;
	std::unique_ptr<IResolvable<PatternArgs, HapticFrame>> _patternResolver;
	std::unique_ptr<IResolvable<ExperienceArgs, HapticSample>> _experienceResolver;
	const Loader _loader;
	
};




class SequenceResolver : public IResolvable<SequenceArgs, HapticEffect>
{
public:
	SequenceResolver(std::unique_ptr<unordered_map<string, vector<SequenceItem>>> loadedFiles);
	~SequenceResolver();
	vector<HapticEffect> Resolve(SequenceArgs args) override;
private:
	HapticCache<HapticEffect> _cache;
	std::unique_ptr<unordered_map<string, vector<SequenceItem>>> _loadedFiles;
	HapticEffect transformSequenceItemIntoEffect(const SequenceItem& seq, Location loc);
};

class PatternResolver : public IResolvable<PatternArgs, HapticFrame>
{
public:
	PatternResolver(std::unique_ptr<unordered_map<string, vector<Frame>>> loadedFiles, std::unique_ptr<IResolvable<SequenceArgs, HapticEffect>> seq);
	~PatternResolver();
	vector<HapticFrame> Resolve(PatternArgs args) override;
private:
	HapticCache<HapticFrame> _cache;
	std::unique_ptr<unordered_map<string, vector<Frame>>> _loadedFiles;
	std::unique_ptr<IResolvable<SequenceArgs, HapticEffect>> _seqResolver;
	HapticFrame transformFrameToHapticFrame(const Frame& frame, Side side) const;
	static Side ComputeSidePrecedence(Side inputSide, Side programmaticSide);
};

class ExperienceResolver : public IResolvable<ExperienceArgs, HapticSample>
{
public:
	ExperienceResolver(std::unique_ptr<unordered_map<string, vector<Moment>>>, std::unique_ptr<IResolvable<PatternArgs, HapticFrame>> pat);
	~ExperienceResolver();
	vector<HapticSample> Resolve(ExperienceArgs args) override;
private:
	HapticCache<HapticSample> _cache;
	std::unique_ptr<unordered_map<string, vector<Moment>>> _loadedFiles;
	std::unique_ptr<IResolvable<PatternArgs, HapticFrame>> _patResolver;
	HapticSample transformMomentToHapticSample(Moment moment, Side side) const;
};
