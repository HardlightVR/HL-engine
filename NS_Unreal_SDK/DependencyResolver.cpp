#include "DependencyResolver.h"
#include "Locator.h"
#include <iostream>


class HapticsNotLoadedException : public std::runtime_error {
public:
	HapticsNotLoadedException(const HapticArgs& args) : std::runtime_error(std::string("Attempted to resolve " + args.ToString() + ", but it was not loaded so failed.").c_str()) {}

};



DependencyResolver::DependencyResolver(const std::string& basePath)
{
	_sequenceResolver = std::make_unique<IResolvable<SequenceArgs, HapticEffect>>(new SequenceResolver());
	_patternResolver = std::make_unique<IResolvable<PatternArgs, HapticFrame>>(new PatternResolver());

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


SequenceResolver::SequenceResolver(std::unique_ptr<unordered_map<string, vector<SequenceItem>>> loadedFiles) :
	_loadedFiles(std::move(loadedFiles))
{
	
}

SequenceResolver::~SequenceResolver()
{
}

vector<HapticEffect> SequenceResolver::Resolve(SequenceArgs args)
{
	if (_loadedFiles->find(args.Name) != _loadedFiles->end())
	{
		throw HapticsNotLoadedException(args);
	}

	if (_cache.Contains(args))
	{
		return _cache.Get(args);
	}

	vector<SequenceItem> inputItems = _loadedFiles->at(args.Name);
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

PatternResolver::PatternResolver(std::unique_ptr<unordered_map<string, vector<Frame>>> loadedFiles, std::unique_ptr<IResolvable<SequenceArgs, HapticEffect>> seq)
	:_loadedFiles(std::move(loadedFiles)), _seqResolver(std::move(seq))
{
}


PatternResolver::~PatternResolver()
{
}

vector<HapticFrame> PatternResolver::Resolve(PatternArgs args)
{
	if (_loadedFiles->find(args.Name) != _loadedFiles->end())
	{
		throw HapticsNotLoadedException(args);
	}

	if (_cache.Contains(args))
	{
		return _cache.Get(args);
	}

	std::vector<HapticFrame> outFrames;
	for (auto frame : _loadedFiles->at(args.Name))
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

ExperienceResolver::ExperienceResolver(std::unique_ptr<unordered_map<string, vector<Moment>>> files, std::unique_ptr<IResolvable<PatternArgs, HapticFrame>> pat):
	_loadedFiles(std::move(files)), _patResolver(std::move(pat))
{
}

ExperienceResolver::~ExperienceResolver()
{
}

vector<HapticSample> ExperienceResolver::Resolve(ExperienceArgs args)
{
	if (_loadedFiles->find(args.Name) != _loadedFiles->end())
	{
		throw HapticsNotLoadedException(args);
	}

	if (_cache.Contains(args))
	{
		return _cache.Get(args);
	}

	std::vector<HapticSample> outSamples;
	for (auto moment : _loadedFiles->at(args.Name))
	{
		outSamples.push_back(transformMomentToHapticSample(moment, args.Side));
	}

	_cache.Cache(args, outSamples);
	return outSamples;
}

HapticSample ExperienceResolver::transformMomentToHapticSample(Moment moment, Side side) const
{
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
