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




class SequenceLoader : public IHapticLoadingStrategy
{
public:
	SequenceLoader(shared_ptr<Parser>, unordered_map<string, vector<SequenceItem>>&);
	~SequenceLoader();
	bool Load(const HapticFileInfo& fileInfo) override;
private:
	shared_ptr<Parser> _parser;
	unordered_map<string, vector<SequenceItem>>& _sequences;
};

class PatternLoader : public IHapticLoadingStrategy
{
public:
	PatternLoader(shared_ptr<Parser>, unique_ptr<SequenceLoader>, unordered_map<string, vector<Frame>>&);
	~PatternLoader();
	bool Load(const HapticFileInfo& fileInfo) override;
private:
	unique_ptr<SequenceLoader> _sequenceLoader;
	shared_ptr<Parser> _parser;
	unordered_map<string, vector<Frame>>& _patterns;
	void loadAllSequences(vector<Frame>) const;
};

class ExperienceLoader : public IHapticLoadingStrategy
{
public:
	ExperienceLoader(shared_ptr<Parser>, unique_ptr<PatternLoader>, unordered_map<string, vector<Moment>>&, unordered_map<string, vector<Frame>>&);
	~ExperienceLoader();
	bool Load(const HapticFileInfo& fileInfo) override;
private:
	unique_ptr<PatternLoader> _patternLoader;
	shared_ptr<Parser> _parser;
	unordered_map<string, vector<Moment>>& _experiences;
	unordered_map<string, vector<Frame>>& _patterns;
	void loadExperience(const std::string& id, boost::filesystem::path path) const;
	float getLatestTime(const std::string& patternName) const;
};
class Loader
{
public:
	Loader(const std::string& basePath);
	~Loader();
	unordered_map<string, vector<SequenceItem>> Sequences;
	unordered_map<string, vector<Moment>>  Experiences;
	unordered_map<string, vector<Frame>> Patterns;

private:
	Parser _parser;
	unique_ptr<PatternLoader> _patternLoader;
	unique_ptr<ExperienceLoader> _experienceLoader;
	unique_ptr<SequenceLoader> _sequenceLoader;



};