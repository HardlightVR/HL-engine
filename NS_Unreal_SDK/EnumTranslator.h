#pragma once
#include "StdAfx.h"
#include "Enums.h"
#include <unordered_map>
#include "boost/bimap.hpp"

typedef boost::bimap<Effect, const char*> EffectMap;
typedef boost::bimap<Location,const char*> LocationMap;


class EnumTranslator
{
public:
	EnumTranslator();
	~EnumTranslator();
	std::string ToString(Location loc);
	std::string ToString(Effect effect);
	//Effect ToEffect(const std::string& effect, Effect defaultEffect);
	Effect ToEffect(std::string effect) const;
//	Location ToLocation(const std::string& location, Location defaultLocation);
	Location ToLocation(std::string location) const;
private:
	
	void init_locations();
	void init_effects();
	EffectMap _effectMap;
	LocationMap _locationMap;
};

