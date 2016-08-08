#include "DependencyResolver.h"



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
}
