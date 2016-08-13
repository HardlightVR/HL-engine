#pragma once
#include "IJsonSerializable.h"
#include <vector>
#include <unordered_map>

#include "HapticDirectoryTools.h"
#include "Enums.h"
using namespace HapticDirectoryTools;


class SequenceItem;
class Frame;
class Sample;

enum class HapticFileType
{
	Pattern = 0,
	Sequence = 1,
	Experience = 2
};

class PackageNotFoundException : public std::runtime_error {
public:
	PackageNotFoundException(const std::string& package) : std::runtime_error(std::string("The package " + package + " was not found!").c_str()) {}
	
};

std::string GetFileType(HapticFileType ftype);

class Parser
{
public:
	Parser();
	Parser(const std::string& basePath);
	~Parser();
	void SetBasePath(const std::string& basePath);
	void EnumerateHapticFiles();
	boost::filesystem::path GetDirectory(std::string package);
	void Traverse(EnumNode node, std::string prefix);
	std::vector<SequenceItem> ParseSequence(boost::filesystem::path);
	std::vector<Frame> ParsePattern(boost::filesystem::path);
	std::vector<Sample> ParseExperience(boost::filesystem::path);
	

private:
	std::string _basePath;
	std::unordered_map<std::string, boost::filesystem::path> _paths;

};

class SequenceItem : public IJsonSerializable {
public : 
	float Time;
	float Duration;
	int Repeat;
	std::string Waveform;
	void Deserialize(const Json::Value& root);
	void Serialize(const Json::Value& root);
};

class JEffect : public IJsonSerializable {
public:
	std::string Sequence;
	std::string Location;
	std::string Side;
	void Deserialize(const Json::Value& root);
	void Serialize(const Json::Value& root);
};

class Frame : public IJsonSerializable {
public:
	float Time;
	std::vector<JEffect> FrameSet;
	void Deserialize(const Json::Value& root);
	void Serialize(const Json::Value& root);
};


class Pattern : public IJsonSerializable {
public:
	std::vector<Frame> Frames;
	void Deserialize(const Json::Value& root);
	void Serialize(const Json::Value& root);
};



class Sample : public IJsonSerializable
{
public:
	Sample();
	~Sample();
	unsigned int Repeat;
	float Time;
	std::string Pattern;
	std::string Side;
	void Deserialize(const Json::Value& root) override;
	void Serialize(const Json::Value& root) override;
};