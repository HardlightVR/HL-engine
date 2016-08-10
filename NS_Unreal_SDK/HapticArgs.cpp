#include "HapticArgs.h"


#include "Enums.h"



PatternArgs::PatternArgs(const std::string& name, ::Side side): Side(side), Name(name)
{
	
}

PatternArgs::~PatternArgs()
{
}

int PatternArgs::GetCombinedHash() const
{
	std::size_t s = 17;
    hash_combine(s, this->Name, this->Side);
	return s;
}

SequenceArgs::SequenceArgs(const std::string& name, ::Location loc) : Location(loc), Name(name)
{
}

SequenceArgs::~SequenceArgs()
{
}

int SequenceArgs::GetCombinedHash() const
{
	std::size_t s = 17;
	hash_combine(s, this->Name, this->Location);
	return s;
}

ExperienceArgs::ExperienceArgs(const std::string& name, ::Side side) : Side(side), Name(name)
{
}

ExperienceArgs::~ExperienceArgs()
{
}

int ExperienceArgs::GetCombinedHash() const
{
	std::size_t s = 17;
	hash_combine(s, this->Name, this->Side);
	return s;
}
