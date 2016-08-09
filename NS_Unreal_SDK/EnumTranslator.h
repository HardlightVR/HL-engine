#pragma once
#include "StdAfx.h"
#include "Enums.h"
#include <unordered_map>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/list_of.hpp>
using namespace boost::bimaps;

typedef bimap<Effect, std::string> EffectMap;
typedef bimap<Location, std::string> LocationMap;


class EnumTranslator
{
public:
	EnumTranslator();
	~EnumTranslator();
	std::string ToString(Location loc);
	std::string ToString(Effect effect);
	Effect ToEffect( std::string effect, Effect defaultEffect);
	Effect ToEffect(std::string effect);
	Location ToLocation(std::string location, Location defaultLocation);
	Location ToLocation(std::string location);
private:
	
	void init_locations();
	void init_effects();
	EffectMap _effectMap;
	LocationMap _locationMap;
};

