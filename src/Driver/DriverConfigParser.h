#pragma once

#include <string>
#include <vector>
#include <boost/variant.hpp>
#include "PluginAPI.h"
namespace Parsing {
	enum class Concept {
		Unknown = 0,
		Suit,
		Gun,
		Controller
	};



	enum class RegionType {
		Unknown = 0,
		Single, 
		Strip
	};

	

	struct LocationDescriptor {
		double height;
		double rotation;

		LocationDescriptor();
	};
	struct SingleRegionDescriptor {
		std::string name;
		nsvr_bodypart bodypart;
		LocationDescriptor location;

		SingleRegionDescriptor();
	};

	struct MultiRegionDescriptor {
		std::string name;
		nsvr_bodypart bodypart;
		LocationDescriptor location_start;
		LocationDescriptor location_end;
		uint32_t count;

		MultiRegionDescriptor();

	};

	struct BodyGraphDescriptor {
		using RegionDescriptor = boost::variant<SingleRegionDescriptor, MultiRegionDescriptor>;
		std::vector<RegionDescriptor> regions;

		BodyGraphDescriptor();
	}; 
	
	struct ManifestDescriptor {
		std::string pluginName;
		uint32_t version;
		Parsing::Concept concept;
		BodyGraphDescriptor bodygraph;

		ManifestDescriptor();

	};

	LocationDescriptor lerp(const Parsing::LocationDescriptor& a, const Parsing::LocationDescriptor& b, float percentage);
	bool deserialize(LocationDescriptor& location, const Json::Value& json, std::string& error);
	bool deserialize(SingleRegionDescriptor& descriptor, const Json::Value& json, std::string& error);
	bool deserialize(MultiRegionDescriptor& descriptor, const Json::Value& json, std::string& error);
	bool deserialize(BodyGraphDescriptor& descriptor, const Json::Value& json, std::string& error);
	bool deserialize(ManifestDescriptor& descriptor, const Json::Value& json, std::string& error);

	boost::optional<Parsing::ManifestDescriptor> ParseConfig(const std::string & path);
}

