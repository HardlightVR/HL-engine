#pragma once
#include <unordered_map>
#include <memory>
#include "HapticClasses.h"
#include "flatbuff_defs\HapticPacket_generated.h"
class HapticCache2
{
public:
	HapticCache2();
	~HapticCache2();
	void AddSequence(std::string name, std::vector<HapticEffect> effects);
	void AddPattern(std::string name, std::vector<HapticFrame> frames);
	void AddExperience(std::string name, std::vector<HapticSample> samples);
	bool ContainsSequence(std::string name);
	bool ContainsPattern(std::string name);
	bool ContainsExperience(std::string name);
	std::vector<HapticEffect> GetSequence(std::string name);
	std::vector<HapticFrame> GetPattern(std::string name);
	std::vector<HapticSample> GetExperience(std::string name);
	void Clear();
private:
	std::unordered_map<std::string, std::vector<HapticEffect>> _sequences;
	std::unordered_map<std::string, std::vector<HapticFrame>> _patterns;
	std::unordered_map<std::string, std::vector<HapticSample>> _experiences;


};

template<class T>
inline T HapticCache2::Get(NullSpace::HapticFiles::FileType ftype, std::string name)
{
	if (ftype == NullSpace::HapticFiles::FileType::FileType_Pattern) {
		return _patterns.at(name);
	}
	else if (ftype == NullSpace::HapticFiles::FileType::FileType_Sequence) {
		return _sequences.at(name);
	}
	else if (ftype == NullSpace::HapticFiles::FileTipe::FileType_Experience) {
		return _experiences.at(name);
	}
}
