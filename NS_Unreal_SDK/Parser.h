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
	Parser(std::string);
	~Parser();
	void SetBasePath(std::string);
	void EnumerateHapticFiles();
	std::string GetDirectory(std::string package);
	void Traverse(EnumNode node, std::string prefix);
private:
	std::string _basePath;
	std::unordered_map<std::string, std::string> _paths;

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

class Effect : public IJsonSerializable {
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
	std::vector<Effect> FrameSet;
	void Deserialize(const Json::Value& root);
	void Serialize(const Json::Value& root);
};


class Pattern : public IJsonSerializable {
public:
	std::vector<Frame> Frames;
	void Deserialize(const Json::Value& root);
	void Serialize(const Json::Value& root);
};