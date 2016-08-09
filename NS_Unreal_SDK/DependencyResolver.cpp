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

Location DependencyResolver::ComputeLocationSide(JsonLocation location, Side side)
{

	switch (location)
	{
	case JsonLocation::Shoulder:
		return side == Side::Left ? Location::Shoulder_Left : Location::Shoulder_Right;
	case JsonLocation::Upper_Back:
		return side == Side::Left ? Location::Upper_Back_Left : Location::Upper_Back_Right;
	case JsonLocation::Lower_Ab:
		return side == Side::Left ? Location::Lower_Ab_Left : Location::Lower_Ab_Right;
	case JsonLocation::Mid_Ab:
		return side == Side::Left ? Location::Mid_Ab_Left : Location::Mid_Ab_Right;
	case JsonLocation::Upper_Ab:
		return side == Side::Left ? Location::Upper_Ab_Left : Location::Upper_Ab_Right;
	case JsonLocation::Chest:
		return side == Side::Left ? Location::Chest_Left : Location::Chest_Right;
	case JsonLocation::Upper_Arm:
		return side == Side::Left ? Location::Upper_Arm_Left : Location::Upper_Arm_Right;
	case JsonLocation::Forearm:
		return side == Side::Left ? Location::Forearm_Left : Location::Forearm_Right;
	default:
		return Location::Error;
	}
}


SequenceResolver::SequenceResolver(unordered_map<string, vector<SequenceItem>> loadedFiles) :
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

HapticEffect SequenceResolver::transformSequenceItemIntoEffect(const SequenceItem& seq, Location loc)
{
	//TODO: priority and explain default values
	Effect effect = Locator::getTranslator().ToEffect(seq.Waveform, Effect::Buzz_100);
	return HapticEffect(Effect::Strong_Click_100, loc, seq.Duration, seq.Time, 1);
}

PatternResolver::PatternResolver(unordered_map<string, vector<Frame>> loadedFiles, IResolvable<PatternArgs, HapticFrame>& seq):_loadedFiles(loadedFiles),
_seqResolver(seq)
{
}

PatternResolver::~PatternResolver()
{
}

vector<HapticFrame> PatternResolver::Resolve(PatternArgs args)
{
	if (_loadedFiles.find(args.Name) != _loadedFiles.end())
	{
		throw HapticsNotLoadedException(args);
	}

	if (_cache.Contains(args))
	{
		return _cache.Get(args);
	}

	std::vector<HapticFrame> outFrames;
	for (auto frame : _loadedFiles[args.Name])
	{
		outFrames.push_back(transformFrameToHapticFrame(frame, args.Side));
	}

	_cache.Cache(args, outFrames);
	return outFrames;
}

HapticFrame PatternResolver::transformFrameToHapticFrame(const Frame& frame, Side side)
{
	std::vector<HapticSequence> sequences;
	for (auto inputSequence : frame.FrameSet)
	{
		Side actualSide = ComputeSidePrecedence(Locator::getTranslator().ToSide(inputSequence.Side, Side::NotSpecified), side);
		const std::string& name = inputSequence.Sequence;
		switch (actualSide)
		{
		case Side::Inherit:
			//should not ever happen. Call NullSpace!
			break;
		case Side::Mirror:
			auto left = _seqResolver->Resolve(
				SequenceArgs(name, DependencyResolver::ComputeLocationSide(
					Locator::getTranslator().ToJsonLocation(inputSequence.Location), Side::Left)));

		}
	}
	
}

Side PatternResolver::ComputeSidePrecedence(Side inputSide, Side programmaticSide)
{
	switch (inputSide)
	{
	case Side::NotSpecified:
		return programmaticSide;
	case Side::Inherit:
		return programmaticSide == Side::Inherit ? Side::Mirror : programmaticSide;
	default:
		return inputSide;
	}
}
