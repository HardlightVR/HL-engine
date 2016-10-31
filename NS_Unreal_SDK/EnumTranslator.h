#pragma once
#include "StdAfx.h"
#include "Enums.h"
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
using namespace boost::bimaps;

typedef bimap<Effect, std::string> EffectMap;
typedef bimap<Location, std::string> LocationMap;
typedef bimap<Side, std::string> SideMap;
typedef bimap<JsonLocation, std::string> JsonLocationMap;
typedef bimap<Imu, std::string> ImuMap;
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
	
	std::string ToString(Side side) const;
	Side ToSide(std::string side) const;
	Side ToSide(std::string side, Side defaultSide);

	std::string ToString(JsonLocation jsonLoc) const;
	JsonLocation ToJsonLocation(std::string jsonLoc) const;
	JsonLocation ToJsonLocation(std::string jsonLoc, JsonLocation defaultLocation);

	std::string ToString(Imu imu) const;
	Imu ToImu(std::string imu) const;
	Imu ToImu(std::string imu, Imu defaultImu);
private:
	
	void init_locations();
	void init_effects();
	void init_sides();
	void init_json_locations();
	void init_imus();

	EffectMap _effectMap;
	LocationMap _locationMap;
	SideMap _sideMap;
	JsonLocationMap _jsonLocationMap;
	ImuMap _imuMap;
};

