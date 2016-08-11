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

template<class T>
class IHapticLoadingStrategy
{
public:
	virtual ~IHapticLoadingStrategy()
	{
	}
	virtual bool Load(const HapticFileInfo& fileInfo) = 0;
	virtual T GetLoadedResource(const std::string& key) = 0;
};



class SequenceLoader : public IHapticLoadingStrategy<vector<SequenceItem>>
{
public:
	SequenceLoader(shared_ptr<Parser>);
	~SequenceLoader();
	bool Load(const HapticFileInfo& fileInfo) override;
	vector<SequenceItem> GetLoadedResource(const std::string& key) override;
private:
	shared_ptr<Parser> _parser;
	unordered_map<string, vector<SequenceItem>> _sequences;
};

class PatternLoader : public IHapticLoadingStrategy<vector<Frame>>
{
public:
	PatternLoader(shared_ptr<Parser>, shared_ptr<SequenceLoader>);
	~PatternLoader();
	bool Load(const HapticFileInfo& fileInfo) override;
	vector<Frame> GetLoadedResource(const std::string& key) override;
private:
	shared_ptr<SequenceLoader> _sequenceLoader;
	shared_ptr<Parser> _parser;
	unordered_map<string, vector<Frame>> _patterns;
	void loadAllSequences(vector<Frame>) const;
};

class ExperienceLoader : public IHapticLoadingStrategy<vector<Moment>>
{
public:
	ExperienceLoader(shared_ptr<Parser>, shared_ptr<PatternLoader>);
	~ExperienceLoader();
	bool Load(const HapticFileInfo& fileInfo) override;
	vector<Moment> GetLoadedResource(const std::string& key) override;
private:
	shared_ptr<PatternLoader> _patternLoader;
	shared_ptr<Parser> _parser;
	unordered_map<string, vector<Moment>> _experiences;
	
	void loadExperience(std::string id, boost::filesystem::path path);
	float getLatestTime(const std::string& patternName) const;
};
class Loader
{
public:
	Loader(const std::string& basePath);
	~Loader();
	shared_ptr<PatternLoader> GetPatternLoader() const;
	shared_ptr<SequenceLoader> GetSequenceLoader() const;
	shared_ptr<ExperienceLoader> GetExperienceLoader() const;
	bool Load(const HapticFileInfo& fileInfo) const;

private:
	shared_ptr<Parser> _parser;
	shared_ptr<PatternLoader> _patternLoader;
	shared_ptr<ExperienceLoader> _experienceLoader;
	shared_ptr<SequenceLoader> _sequenceLoader;



};