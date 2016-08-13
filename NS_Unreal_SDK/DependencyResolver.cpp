#include "DependencyResolver.h"
#include "Locator.h"
#include <iostream>
#include "HapticFileInfo.h"

class HapticsNotLoadedException : public std::runtime_error {
public:
	HapticsNotLoadedException(const HapticArgs& args) : std::runtime_error(std::string("Attempted to resolve " + args.ToString() + ", but it was not loaded so failed.").c_str()) {}

};



DependencyResolver::DependencyResolver(const std::string& basePath): _loader(basePath)
{
	_sequenceResolver = make_shared<SequenceResolver>(_loader.GetSequenceLoader());
	_patternResolver = make_shared<PatternResolver>(_sequenceResolver, _loader.GetPatternLoader());
	_experienceResolver = make_shared<ExperienceResolver>(_patternResolver, _loader.GetExperienceLoader());


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

std::vector<HapticEffect> DependencyResolver::ResolveSequence(const std::string & name, Location location) const
{
	return _sequenceResolver->Resolve(SequenceArgs(name, location));
}

std::vector<HapticFrame> DependencyResolver::ResolvePattern(const std::string& name, Side side) const
{
	return _patternResolver->Resolve(PatternArgs(name, side));
}

std::vector<HapticSample> DependencyResolver::ResolveSample(const std::string& name, Side side) const
{
	return _experienceResolver->Resolve(ExperienceArgs(name, side));
}

bool DependencyResolver::Load(const HapticFileInfo& fileInfo) const
{
	return _loader.Load(fileInfo);
}


SequenceResolver::~SequenceResolver()
{
}

SequenceResolver::SequenceResolver(shared_ptr<SequenceLoader> loader) :_sequenceLoader(loader)
{
}

vector<HapticEffect> SequenceResolver::Resolve(SequenceArgs args)
{
	//if (_loadedFiles.find(args.Name) != _loadedFiles.end())
	//{
	//	throw HapticsNotLoadedException(args);
	//}

	if (_cache.Contains(args))
	{
		return _cache.Get(args);
	}

	vector<SequenceItem> inputItems = _sequenceLoader->GetLoadedResource(args.Name);
	vector<HapticEffect> outputEffects;
	outputEffects.reserve(inputItems.size());
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
	return HapticEffect(effect, loc, seq.Duration, seq.Time, 1);
}

PatternResolver::PatternResolver(shared_ptr<IResolvable<SequenceArgs, HapticEffect>>  seq, shared_ptr<PatternLoader> p)
: _patternLoader(p), _seqResolver(seq)
{
}




PatternResolver::~PatternResolver()
{
}

vector<HapticFrame> PatternResolver::Resolve(PatternArgs args)
{
	////if (_loadedFiles.find(args.Name) != _loadedFiles.end())
	//{
	//	throw HapticsNotLoadedException(args);
//	}

	if (_cache.Contains(args))
	{
		return _cache.Get(args);
	}

	std::vector<HapticFrame> outFrames;
	for (auto frame : _patternLoader->GetLoadedResource(args.Name))
	{
		outFrames.push_back(transformFrameToHapticFrame(frame, args.Side));
	}

	_cache.Cache(args, outFrames);
	return outFrames;
}

HapticFrame PatternResolver::transformFrameToHapticFrame(const Frame& frame, Side side) const
{
	std::vector<HapticSequence> sequences;
	for (auto inputSequence : frame.FrameSet)
	{
		Side actualSide = ComputeSidePrecedence(Locator::getTranslator().ToSide(inputSequence.Side, Side::NotSpecified), side);
		const std::string& name = inputSequence.Sequence;
		JsonLocation location = Locator::getTranslator().ToJsonLocation(inputSequence.Location);

		switch (actualSide)
		{
		case Side::Inherit:
			//should not ever happen. Call NullSpace!
			std::cout << "(Error: impossible) Please email casey@nullspacevr.com if you see this message" << "\n";
			break;
		case Side::Mirror:
			{
				auto left = _seqResolver->Resolve(
					SequenceArgs(name, DependencyResolver::ComputeLocationSide(location, Side::Left))
				);
				sequences.push_back(HapticSequence(left));
				auto right = _seqResolver->Resolve(
					SequenceArgs(name, DependencyResolver::ComputeLocationSide(location, Side::Right))
				);
				sequences.push_back(HapticSequence(right));
			}
			break;
		default:
			{
				auto specific = _seqResolver->Resolve(
					SequenceArgs(name, DependencyResolver::ComputeLocationSide(location, actualSide))
				);
				sequences.push_back(HapticSequence(specific));
			}
			break;
		}
	}

	return HapticFrame(frame.Time, sequences);
	
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

ExperienceResolver::ExperienceResolver(shared_ptr<IResolvable<PatternArgs, HapticFrame>> pat, shared_ptr<ExperienceLoader> el):
	_experienceLoader(el), _patResolver(pat)
{
}

ExperienceResolver::~ExperienceResolver()
{
}

vector<HapticSample> ExperienceResolver::Resolve(ExperienceArgs args)
{
	//if (_loadedFiles.find(args.Name) != _loadedFiles.end())
	//{
	//	throw HapticsNotLoadedException(args);
	//}

	if (_cache.Contains(args))
	{
		return _cache.Get(args);
	}

	std::vector<HapticSample> outSamples;
	for (auto moment : _experienceLoader->GetLoadedResource(args.Name))
	{
		outSamples.push_back(transformMomentToHapticSample(moment, args.Side));
	}

	_cache.Cache(args, outSamples);
	return outSamples;
}

HapticSample ExperienceResolver::transformMomentToHapticSample(Moment moment, Side side) const
{
	//todo: step through this
	vector<HapticFrame>* frames = nullptr;
	if (moment.Side != Side::NotSpecified && moment.Side != Side::Inherit)
	{
		//then resolve the pattern using that file-defined side.
		*frames = _patResolver->Resolve(PatternArgs(moment.Name, moment.Side));
	}
	else
	{
		//else, use the programmatic side given at runtime by the programmer
		*frames = _patResolver->Resolve(PatternArgs(moment.Name, side));
	}
	return HapticSample(moment.Time, *frames, 1);

}
