#include "Loader.h"
#include "HapticFileInfo.h"
#include <iostream>
#include "Locator.h"

using namespace boost::filesystem;

Loader::Loader(const std::string& basePath): _parser(basePath)
{
	auto sharedParser = std::make_shared<Parser>(_parser);
	_sequenceLoader = std::make_shared<SequenceLoader>(sharedParser);
	_patternLoader = std::make_shared<PatternLoader>(sharedParser, _sequenceLoader);
	_experienceLoader = std::make_shared<ExperienceLoader>(sharedParser, _patternLoader);
}


Loader::~Loader()
{
}

shared_ptr<PatternLoader> Loader::GetPatternLoader() const
{
	return _patternLoader;
}

shared_ptr<SequenceLoader> Loader::GetSequenceLoader() const
{
	return _sequenceLoader;
}

shared_ptr<ExperienceLoader> Loader::GetExperienceLoader() const
{
	return _experienceLoader;
}

SequenceLoader::SequenceLoader(shared_ptr<Parser> p)
	:_parser(p) {
}

SequenceLoader::~SequenceLoader()
{
}

bool SequenceLoader::Load(const HapticFileInfo& fileInfo)
{
	if (_sequences.find(fileInfo.FullId) != _sequences.end()) {
		return true;
	}

	auto validNames = fileInfo.GetValidFileNames();
	path directory = _parser->GetDirectory(fileInfo.FullyQualifiedPackage) /= fileInfo.GetDirectory();
	for (auto name : validNames)
	{
		
		auto path = directory /= name;
		if (exists(path)) {
			vector<SequenceItem> sequence = _parser->ParseSequence(path);
			_sequences.at(fileInfo.FullId) = sequence;
			return true;
		} 
		
	}

	std::cout << "Failed to load " << fileInfo.ToString() << ": file not found (in package " << fileInfo.FullyQualifiedPackage << ")\n";
	return false;
}

vector<SequenceItem> SequenceLoader::GetLoadedResource(const std::string& key)
{
	return _sequences.at(key);
}

PatternLoader::PatternLoader(shared_ptr<Parser> p, shared_ptr<SequenceLoader> seq)
:_sequenceLoader(std::move(seq)), _parser(p){
}

PatternLoader::~PatternLoader()
{
}

bool PatternLoader::Load(const HapticFileInfo& fileInfo)
{
	if (_patterns.find(fileInfo.FullId) != _patterns.end()) {
		return true;
	}

	auto validNames = fileInfo.GetValidFileNames();
	path directory = _parser->GetDirectory(fileInfo.FullyQualifiedPackage) /= fileInfo.GetDirectory();
	for (auto name : validNames)
	{

		auto path = directory /= name;
		if (exists(path)) {
			vector<Frame> pattern = _parser->ParsePattern(path);
			_patterns.at(fileInfo.FullId) = pattern;
			loadAllSequences(pattern);
			return true;
		}

	}
	return false;
}

vector<Frame> PatternLoader::GetLoadedResource(const std::string & key)
{
	return _patterns.at(key);
}

void PatternLoader::loadAllSequences(vector<Frame> pattern) const
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

ExperienceLoader::ExperienceLoader(shared_ptr<Parser> p, shared_ptr<PatternLoader> pat)
	:_patternLoader(std::move(pat)), _parser(p)
{
}


ExperienceLoader::~ExperienceLoader()
{
}

bool ExperienceLoader::Load(const HapticFileInfo& fileInfo)
{
	if (_experiences.find(fileInfo.FullId) != _experiences.end()) {
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

vector<Moment> ExperienceLoader::GetLoadedResource(const std::string & key)
{
	return _experiences.at(key);
}

void ExperienceLoader::loadExperience(const std::string& id, path path) const
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
	
	auto pattern = _patternLoader->GetLoadedResource(patternName);
	float latestTime = 0;
	for (auto frame : pattern)
	{
		latestTime = max(latestTime, frame.Time);
	}
	return latestTime;

}
