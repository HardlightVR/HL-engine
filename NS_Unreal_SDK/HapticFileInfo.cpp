#include "HapticFileInfo.h"


std::vector<std::string> Glue(std::string firstPart, std::string separator, const std::vector<std::string>& lastParts) 
{
	std::vector<std::string> results;
	for (auto part : lastParts)
	{
		results.push_back(firstPart + separator + part);
	}
	return results;
}


const std::string HapticFileInfo::_packageSeparator = ":";
std::string HapticFileInfo::getPackage(std::string thing)
{
	std::vector<std::string> parts;
	boost::split(parts, thing, boost::is_any_of(_packageSeparator));
	auto range = boost::make_iterator_range(parts.begin(), parts.end() - 1);
	return boost::join(range, _packageSeparator);
}

std::string HapticFileInfo::getName(std::string thing)
{
	std::vector<std::string> parts;
	boost::split(parts, thing, boost::is_any_of(_packageSeparator));
	return parts[parts.size() - 1];
}

SequenceFileInfo::SequenceFileInfo(std::string fullId):HapticFileInfo(HapticFileType::Sequence, fullId)
{
}

SequenceFileInfo::~SequenceFileInfo()
{
}


std::vector<std::string> SequenceFileInfo::GetValidFileNames() const
{
	return Glue(Name, ".", { "seq", ".sequence" });
}

std::string SequenceFileInfo::GetDirectory() const
{
	return "sequences";
}

PatternFileInfo::PatternFileInfo(std::string fullId):HapticFileInfo(HapticFileType::Pattern, fullId)
{
}

PatternFileInfo::~PatternFileInfo()
{
}

std::vector<std::string> PatternFileInfo::GetValidFileNames() const
{
	return Glue(Name, ".", { "pat", "pattern" });
}

std::string PatternFileInfo::GetDirectory() const
{
	return "patterns";
}

ExperienceFileInfo::ExperienceFileInfo(std::string fullId):HapticFileInfo(HapticFileType::Experience, fullId)
{

}

ExperienceFileInfo::~ExperienceFileInfo()
{
}

std::vector<std::string> ExperienceFileInfo::GetValidFileNames() const
{
	return Glue(Name, ".", { "exp", "experience" });
}

std::string ExperienceFileInfo::GetDirectory() const
{
	return "experiences";
}

HapticFileInfo::HapticFileInfo(HapticFileType ftype, std::string fullId):
	FullyQualifiedPackage(getPackage(fullId)),
	Name(getName(fullId)), 
	FullId(fullId), 
	FileType(ftype)
{


}

HapticFileInfo::~HapticFileInfo()
{
}
