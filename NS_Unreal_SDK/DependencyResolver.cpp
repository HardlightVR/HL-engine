#include "DependencyResolver.h"
#include "Locator.h"
class HapticsNotLoadedException : public std::runtime_error {
public:
	HapticsNotLoadedException(const HapticArgs& args) : std::runtime_error(std::string("Attempted to resolve " + args.ToString() + ", but it was not loaded so failed.").c_str()) {}

};
DependencyResolver::DependencyResolver()
{
}


DependencyResolver::~DependencyResolver()
{
}



SequenceResolver::SequenceResolver(unordered_map<string, vector<SequenceItem>> loadedFiles):
	_loadedFiles(loadedFiles)
{
	
}

SequenceResolver::~SequenceResolver()
{
}

vector<HapticEffect> SequenceResolver::Resolve(SequenceArgs args)
{
	if (_loadedFiles.find(args.Name) != _loadedFiles.end())
	{
		throw HapticsNotLoadedException(args);
	}

	if (_cache.Contains(args))
	{
		return _cache.Get(args);
	}

	vector<SequenceItem> inputItems = _loadedFiles[args.Name];
	vector<HapticEffect> outputEffects(inputItems.size());
	for (auto seqItem : inputItems)
	{
		outputEffects.push_back(transformSequenceItemIntoEffect(seqItem, args.Location));
	}

	_cache.Cache(args, outputEffects);
	return outputEffects;
}

HapticEffect SequenceResolver::transformSequenceItemIntoEffect(SequenceItem seq, Location loc)
{
	//TODO: priority and explain default values
	//Effect effect = Locator::getTranslator().ToEffect(seq.Waveform, Effect::Buzz_100);
	return HapticEffect(Effect::Strong_Click_100, loc, seq.Duration, seq.Time, 1);
}
