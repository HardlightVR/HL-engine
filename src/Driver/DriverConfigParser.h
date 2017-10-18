#pragma once

#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include "json/json-forwards.h"
#include "PluginAPI.h"
namespace Parsing {




	enum class RegionType {
		Unknown = 0,
		Single, 
		Strip
	};

	

	struct LocationDescriptor {
		double height;
		double rotation;

		LocationDescriptor();
		LocationDescriptor(double height, double rot) : height(height), rotation(rot) {}
	};
	struct SingleRegionDescriptor {
		std::string name;
		nsvr_bodypart bodypart;
		LocationDescriptor location;

		SingleRegionDescriptor();
		SingleRegionDescriptor(std::string name, nsvr_bodypart bp, LocationDescriptor loc) :
			name(name), bodypart(bp), location(loc) {}
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

	struct VirtualNodeDescriptor {
		uint32_t id;
		std::string name;
		nsvr_node_type concept;
		std::vector<std::string> regions;
	};
	struct VirtualDeviceDescriptor {
		std::string name;
		nsvr_device_concept concept;
		std::vector<VirtualNodeDescriptor> nodes;
	};

	struct VirtualDeviceList {
		std::vector<VirtualDeviceDescriptor> devices;
	};
	
	struct ManifestDescriptor {
		std::string pluginName;
		uint32_t version;
		BodyGraphDescriptor bodygraph;
		VirtualDeviceList vdevices;
		ManifestDescriptor();

	};
	bool IsProbablyManifest(const std::string& path_string);
	LocationDescriptor lerp(const Parsing::LocationDescriptor& a, const Parsing::LocationDescriptor& b, float percentage);
	
	boost::optional<Parsing::ManifestDescriptor> ParseConfig(const std::string & path);
}

