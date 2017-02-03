#pragma once
#include "Enums.h"
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include "IntermediateHapticFormats.h"
using namespace boost::bimaps;

typedef bimap<Effect, std::string> EffectMap;
typedef bimap<Location, std::string> LocationMap;
class EnumTranslator
{
public:
	EnumTranslator();
	~EnumTranslator();

	std::string ToString(Location loc) const;
	Location ToLocation(std::string location) const;
	Location ToLocation(std::string location, Location defaultLocation);


	std::string ToString(Effect effect) const;
	Effect ToEffect(std::string effect) const;
	Effect ToEffect(std::string effect, Effect defaultEffect);
	

private:
	
	void init_locations();
	void init_effects();

	
	EffectMap _effectMap;
	LocationMap _locationMap;
	

};

