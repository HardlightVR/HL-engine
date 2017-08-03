#pragma once
#include "Enums.h"
#include "AreaFlags.h"
#include "PluginAPI.h"
#include <string>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
using namespace boost::bimaps;

typedef bimap<Effect, std::string> EffectMap;
typedef bimap<Location, std::string> LocationMap;
typedef bimap<Side, std::string> SideMap;
typedef bimap<JsonLocation, std::string> JsonLocationMap;
typedef bimap<Imu, std::string> ImuMap;
typedef bimap<AreaFlag, std::string> AreaMap;
typedef bimap<std::string, uint8_t> EffectFamilyMap;
typedef bimap<nsvr_region, std::string> RegionMap;

class EnumTranslator
{
public:
	EnumTranslator();

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

	AreaFlag ToArea(std::string area, AreaFlag default) const;

	AreaFlag ToArea(std::string area) const;
	std::string ToString(AreaFlag area) const;

	uint32_t ToEffectFamily(std::string effectFamily) const;
	std::string ToString(uint32_t effectFamily) const;

	nsvr_region ToRegion(const std::string& region, nsvr_region default) const;
	std::string ToRegionString(nsvr_region region) const;
private:
	
	void init_locations();
	void init_effects();
	void init_sides();
	void init_json_locations();
	void init_imus();
	void init_areas();
	void init_familymap();
	void init_regions();

	AreaMap _areaMap;
	EffectMap _effectMap;
	LocationMap _locationMap;
	SideMap _sideMap;
	JsonLocationMap _jsonLocationMap;
	ImuMap _imuMap;
	EffectFamilyMap _effectFamilyMap;
	RegionMap _regionMap;
};

