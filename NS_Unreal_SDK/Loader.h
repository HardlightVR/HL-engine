#pragma once
#include <memory>
#include <unordered_map>
#include "Parser.h"

class Parser;
class HapticFileInfo;

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
	Loader();
	~Loader();
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
};
