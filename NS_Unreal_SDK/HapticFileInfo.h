#pragma once
#include <string>
#include "Parser.h"
#include "HapticArgs.h"
#include <boost/algorithm/string.hpp>

class HapticFileInfo
{
public:
	HapticFileInfo(HapticFileType ftype, std::string fullId);
	virtual ~HapticFileInfo();
	const std::string FullyQualifiedPackage;
	const std::string Name;
	const std::string FullId;
	const HapticFileType FileType;
	virtual std::string GetDirectory() const
	{
		return "";
	}
	int GetHashCode() const
	{
		std::size_t seed = 17;
		hash_combine(seed, FullId, FileType);
		return seed;
	}
	std::string ToString() const
	{
		std::vector<std::string> vec = { FullyQualifiedPackage, GetFileType(FileType), Name };
		return boost::join(vec, _packageSeparator);
	}

	virtual std::vector<std::string> GetValidFileNames() const
	{
		return std::vector<std::string>();
	}


private:
	static std::string getPackage(std::string thing);
	static std::string getName(std::string thing);
protected:
	static const std::string _packageSeparator;

};

class SequenceFileInfo : public HapticFileInfo
{
public: 
	SequenceFileInfo(std::string fullId);
	~SequenceFileInfo();
	std::vector<std::string> GetValidFileNames() const override;
	std::string GetDirectory() const override;
};


class PatternFileInfo : public HapticFileInfo
{
public:
	PatternFileInfo(std::string fullId);
	~PatternFileInfo();
	std::vector<std::string> GetValidFileNames() const override;
	std::string GetDirectory() const override;
};

class ExperienceFileInfo : public HapticFileInfo
{
public:
	ExperienceFileInfo(std::string fullId);
	~ExperienceFileInfo();
	std::vector<std::string> GetValidFileNames() const override;
	std::string GetDirectory() const override;
};