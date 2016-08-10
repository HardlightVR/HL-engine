#include "Loader.h"
#include "HapticFileInfo.h"
#include <iostream>
#include "Locator.h"

using namespace boost::filesystem;

Loader::Loader(std::shared_ptr<Parser> parser): _parser(parser)
{

}


Loader::~Loader()
{
}

SequenceLoader::SequenceLoader(shared_ptr<Parser> p, shared_ptr<unordered_map<string, vector<SequenceItem>>> s)
	:_parser(p), _sequences(s) {
}

SequenceLoader::~SequenceLoader()
{
}

bool SequenceLoader::Load(const HapticFileInfo& fileInfo)
{
	if (_sequences->find(fileInfo.FullId) != _sequences->end()) {
		return true;
	}

	auto validNames = fileInfo.GetValidFileNames();
	path directory = _parser->GetDirectory(fileInfo.FullyQualifiedPackage) /= fileInfo.GetDirectory();
	for (auto name : validNames)
	{
		
		auto path = directory /= name;
		if (exists(path)) {
			vector<SequenceItem> sequence = _parser->ParseSequence(path);
			_sequences->at(fileInfo.FullId) = sequence;
			return true;
		} 
		
	}

	std::cout << "Failed to load " << fileInfo.ToString() << ": file not found (in package " << fileInfo.FullyQualifiedPackage << ")\n";
	return false;
}

PatternLoader::PatternLoader(shared_ptr<Parser> p, unique_ptr<SequenceLoader> seq, shared_ptr<unordered_map<string, vector<Frame>>> files)
:_sequenceLoader(std::move(seq)), _parser(p), _patterns(files){
}

PatternLoader::~PatternLoader()
{
}

bool PatternLoader::Load(const HapticFileInfo& fileInfo)
{
	if (_patterns->find(fileInfo.FullId) != _patterns->end()) {
		return true;
	}

	auto validNames = fileInfo.GetValidFileNames();
	path directory = _parser->GetDirectory(fileInfo.FullyQualifiedPackage) /= fileInfo.GetDirectory();
	for (auto name : validNames)
	{

		auto path = directory /= name;
		if (exists(path)) {
			vector<Frame> pattern = _parser->ParsePattern(path);
			_patterns->at(fileInfo.FullId) = pattern;
			loadAllSequences(pattern);
			return true;
		}

	}
	return false;
}

void PatternLoader::loadAllSequences(vector<Frame> pattern)
{
	for (auto frame : pattern)
	{
		for (auto sequence : frame.FrameSet)
		{
			SequenceFileInfo info(sequence.Sequence);
			_sequenceLoader->Load(info);
		}
	}
}

ExperienceLoader::ExperienceLoader(shared_ptr<Parser> p, unique_ptr<PatternLoader> pat, shared_ptr<unordered_map<string, vector<Moment>>> moments, shared_ptr<unordered_map<string, vector<Frame>>> frames)
	:_patternLoader(std::move(pat)), _parser(p), _experiences(moments), _patterns(frames)
{
}


ExperienceLoader::~ExperienceLoader()
{
}

bool ExperienceLoader::Load(const HapticFileInfo& fileInfo)
{
	if (_experiences->find(fileInfo.FullId) != _experiences->end()) {
		return true;
	}

	auto validNames = fileInfo.GetValidFileNames();
	path directory = _parser->GetDirectory(fileInfo.FullyQualifiedPackage) /= fileInfo.GetDirectory();
	for (auto name : validNames)
	{

		auto path = directory /= name;
		if (exists(path)) {
			loadExperience(fileInfo.FullId, path);
			return true;
		}

	}
	return false;
}

void ExperienceLoader::loadExperience(const std::string& id, boost::filesystem::path path)
{
	vector<Sample> unprocessedSamples = _parser->ParseExperience(path);
	vector<Moment> processedSamples(unprocessedSamples.size());

	for (const auto sample : unprocessedSamples)
	{
		std::string patternName = sample.Pattern;
		PatternFileInfo patInfo(patternName);
		_patternLoader->Load(patInfo);
		Side sideEnum = Locator::getTranslator().ToSide(sample.Side);
		processedSamples.push_back(Moment(patternName, sample.Time, sideEnum));
		if (sample.Repeat > 0)
		{
			float offset = 0.1f + getLatestTime(patternName);
			for (std::size_t repetition = 1; repetition < sample.Repeat; ++repetition)
			{
				processedSamples.push_back(Moment(patternName, sample.Time + offset * repetition, sideEnum));
			}
		}
	}


}

float ExperienceLoader::getLatestTime(const std::string& patternName) const
{
	auto pattern = _patterns->at(patternName);
	float latestTime = 0;
	for (auto frame : pattern)
	{
		latestTime = max(latestTime, frame.Time);
	}
	return latestTime;

}
