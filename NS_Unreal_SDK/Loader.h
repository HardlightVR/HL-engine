#pragma once
#include <memory>
#include <unordered_map>
#include "Parser.h"
#include "HapticClasses.h"

class Parser;
class HapticFileInfo;
class PatternLoader;
class SequenceLoader;
class ExperienceLoader;

using namespace std;
class IHapticLoadingStrategy
{
public:
	virtual ~IHapticLoadingStrategy()
	{
	}
	virtual bool Load(const HapticFileInfo& fileInfo) = 0;
};

class Loader
{
public:
	Loader(std::shared_ptr<Parser> parser);
	~Loader();
private:
	std::shared_ptr<Parser> _parser;
	std::shared_ptr<PatternLoader> _patternLoader;
	std::shared_ptr<ExperienceLoader> _experienceLoader;
	std::shared_ptr<SequenceLoader> _sequenceLoader;

	std::unordered_map<string, vector<SequenceItem>> _sequences;
	Dictionary<string, List<SequenceItem>> _sequences;
	Dictionary<string, List<Moment>> _experiences;
	Dictionary<string, List<Frame>> _patterns;

};


class SequenceLoader : public IHapticLoadingStrategy
{
public:
	SequenceLoader(shared_ptr<Parser>, shared_ptr<unordered_map<string, vector<SequenceItem>>>);
	~SequenceLoader();
	bool Load(const HapticFileInfo& fileInfo) override;
private:
	shared_ptr<Parser> _parser;
	shared_ptr<unordered_map<string, vector<SequenceItem>>> _sequences;
};

class PatternLoader : public IHapticLoadingStrategy
{
public:
	PatternLoader(shared_ptr<Parser>, unique_ptr<SequenceLoader>, shared_ptr<unordered_map<string, vector<Frame>>>);
	~PatternLoader();
	bool Load(const HapticFileInfo& fileInfo) override;
private:
	unique_ptr<SequenceLoader> _sequenceLoader;
	shared_ptr<Parser> _parser;
	shared_ptr<unordered_map<string, vector<Frame>>> _patterns;
	void loadAllSequences(vector<Frame>);
};

class ExperienceLoader : public IHapticLoadingStrategy
{
public:
	ExperienceLoader(shared_ptr<Parser>, unique_ptr<PatternLoader>, shared_ptr<unordered_map<string, vector<Moment>>>, shared_ptr<unordered_map<string, vector<Frame>>>);
	~ExperienceLoader();
	bool Load(const HapticFileInfo& fileInfo) override;
private:
	unique_ptr<PatternLoader> _patternLoader;
	shared_ptr<Parser> _parser;
	shared_ptr<unordered_map<string, vector<Moment>>> _experiences;
	shared_ptr<unordered_map<string, vector<Frame>>> _patterns;
	void loadExperience(const std::string& id, boost::filesystem::path path);
	float getLatestTime(const std::string& patternName) const;
};
