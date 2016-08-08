#pragma once
#include <string>
#include "IJsonSerializable.h"
#include <vector>
#include <unordered_map>
#include <exception>

#include "HapticDirectoryTools.h"

using namespace HapticDirectoryTools;




class PackageNotFoundException : public std::runtime_error {
public:
	PackageNotFoundException(const std::string& package) : std::runtime_error(std::string("The package " + package + " was not found!").c_str()) {}
	
};


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