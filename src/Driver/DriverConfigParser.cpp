#include "stdafx.h"
#include "DriverConfigParser.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include "Locator.h"
#include "PluginAPI.h"
#include "json/json.h"


namespace Parsing {


	bool IsProbablyManifest(const std::string& path_string) {
		boost::filesystem::path path(path_string);
		if (!path.has_stem()) {
			return false;
		}
		std::string stem = path.stem().string();

		return stem.find("_manifest") != std::string::npos;
	}
	static std::unordered_map<std::string, nsvr_device_concept> concept_map = {
		{ "suit", nsvr_device_concept_suit },
		{ "controller", nsvr_device_concept_controller },
		{ "gun", nsvr_device_concept_gun},
		{"gauntlet", nsvr_device_concept_gauntlet},
		{"headwear", nsvr_device_concept_headwear},
		{"sword", nsvr_device_concept_sword}
	};

	static std::unordered_map<std::string, nsvr_node_concept> node_concept_map = {
		{"haptic", nsvr_node_type_haptic},
		{"inertial_tracker", nsvr_node_type_inertial_tracker},
		{"absolute_tracker", nsvr_node_type_absolute_tracker},
		{"led", nsvr_node_type_led}
	};
	double angle_distance(double angle_a, double angle_b) {
		if (angle_a > angle_b) {
			return (360 - angle_a) + angle_b;
		}
		else {
			return angle_b - angle_a;
		}
	}

	LocationDescriptor lerp(const Parsing::LocationDescriptor& a, const Parsing::LocationDescriptor& b, float percentage) {
		double lerped_height = (a.height * (1.0 - percentage)) + (b.height * percentage);
		double real_lerped_rot = a.rotation + angle_distance(a.rotation, b.rotation)*percentage;
		double z = fmod(real_lerped_rot, 360.0);
		Parsing::LocationDescriptor result;
		result.height = lerped_height;
		result.rotation = z;
		return result;
	}


	bool deserialize(LocationDescriptor& location, const Json::Value& json, std::vector<std::string>& errors) {
		if (!json.isObject()) {
			errors.push_back("Must be object");
			return false;
		}
		location.height = json.get("height", -1.0).asDouble();
		if (location.height == -1.0) {
			errors.push_back("Must have key 'height'");
			return false;
		}
		if (location.height < 0 || location.height > 1.0) {
			errors.push_back("Key 'height' must have a value in the range [0, 1.0]");
			return false;
		}

		location.rotation = json.get("rotation", -1.0).asDouble();
		if (location.rotation == -1.0) {
			errors.push_back("Must have key 'rotation'");
			return false;
		}
		if (location.rotation < 0 || location.rotation > 360) {
			errors.push_back("Key 'rotation' must have a value in the range [0, 360]");
			return false;
		}
		return true;
	}

	bool deserialize(SingleRegionDescriptor& descriptor, const Json::Value& json, std::vector<std::string>& errors) {
		if (!json.isObject()) {
			errors.push_back("Must be object");
			return false;
		}

		descriptor.name = json.get("name", "unknown").asString();
		if (descriptor.name == "unknown") {
			errors.push_back("Must have key 'name'");
			return false;
		}

		std::string rawBodyPart = json.get("bodypart", "unknown").asString();
		if (rawBodyPart == "unknown") {
			errors.push_back("Must have key 'bodypart'");
			return false;
		}

		const auto& translator = Locator::Translator();
		descriptor.bodypart = translator.ToBodypart(rawBodyPart, nsvr_bodypart_unknown);
		if (descriptor.bodypart == nsvr_bodypart_unknown) {
			errors.push_back("Unrecognized bodypart: '" + rawBodyPart + "'");
			return false;
		}




		if (!deserialize(descriptor.location, json["location"], errors)) {
			errors.insert(errors.begin(), "Error(s) parsing location");
			return false;
		}


		return true;
	}

	bool deserialize(MultiRegionDescriptor& descriptor, const Json::Value& json, std::vector<std::string>& errors) {
		if (!json.isObject()) {
			errors.push_back("Must be object");
			return false;
		}

		descriptor.name = json.get("name", "unknown").asString();
		if (descriptor.name == "unknown") {
			errors.push_back("Must have key 'name'");
			return false;
		}

		std::string rawBodyPart = json.get("bodypart", "unknown").asString();
		if (rawBodyPart == "unknown") {
			errors.push_back("Must have key 'bodypart'");
			return false;
		}


		const auto& translator = Locator::Translator();
		descriptor.bodypart = translator.ToBodypart(rawBodyPart, nsvr_bodypart_unknown);
		if (descriptor.bodypart == nsvr_bodypart_unknown) {
			errors.push_back("Unrecognized bodypart: '" + rawBodyPart + "'");
			return false;
		}

		descriptor.count = json.get("count", 2).asUInt();


		if (!deserialize(descriptor.location_start, json["location_start"], errors)) {
			errors.insert(errors.begin(), "Error parsing LocationStart");
			return false;
		}

		if (!deserialize(descriptor.location_end, json["location_end"], errors)) {
			errors.insert(errors.begin(), "When parsing LocationEnd");
			return false;
		}

		return true;
	}

	bool deserialize(std::vector<BodyGraphDescriptor::RegionDescriptor>& descriptor, const Json::Value& value, std::vector<std::string>& errors) {
		bool hasLocationKey = !value["location"].isNull();
		bool hasLocationStartKey = !value["location_start"].isNull();
		bool hasLocationEndKey = !value["location_end"].isNull();

		if (!hasLocationKey && !hasLocationStartKey && !hasLocationEndKey) {
			errors.push_back("You must either specify a single location using key 'location', or a multi-location using keys 'location_start' and 'location_end'");
			return false;
		}
		if (hasLocationKey) {
			if (hasLocationStartKey || hasLocationEndKey) {
				errors.push_back("Single locations must only have a single 'location' key");
				return false;
			}
		}
		else {
			if (!(hasLocationStartKey && hasLocationEndKey)) {
				errors.push_back("Multi locations must have the 'location_start' and 'location_end' keys");
				return false;
			}
		}

		if (hasLocationKey) {
			SingleRegionDescriptor single;
			if (!deserialize(single, value, errors)) {
				errors.insert(errors.begin(), "Error parsing SingleRegion");
				return false;
			}
			descriptor.push_back(std::move(single));
		}
		else {
			MultiRegionDescriptor multi;
			if (!deserialize(multi, value, errors)) {
				errors.insert(errors.begin(), "Error parsing MultiRegion");
				return false;
			}
			descriptor.push_back(std::move(multi));
		}

		return true;
	}
	bool deserialize(BodyGraphDescriptor& descriptor, const Json::Value& json, std::vector<std::string>& errors)
	{
		if (!json["regions"].isArray()) {
			errors.push_back("Key 'regions' must be present, and an array");
			return false;
		}

		for (std::size_t i = 0; i < json["regions"].size(); i++) {
			const auto& value = json["regions"][i];
			if (!deserialize(descriptor.regions, value, errors)) {
				errors.insert(errors.begin(), "Error parsing region [" + std::to_string(i) + "]");
				return false;
			}


		}

		return true;

	}

	
	bool deserialize(std::vector<VirtualNodeDescriptor>& descriptors, const Json::Value& json, std::vector<std::string>& errors) {
		bool hasId = json["id"].isUInt();
		bool hasName = json["name"].isString();
		bool hasConcept = json["concept"].isString();
		bool hasRegions = json["regions"].isArray();

		if (!hasId) {
			errors.push_back("Node must have positive int key 'id'");
			return false;
		}
		else if (!hasName) {
			errors.push_back("Node must have string key 'name'");
			return false;
		}
		else if (!hasConcept) {
			errors.push_back("Node must have string key 'concept'");
			return false;
		} 
		else if (!hasRegions) {
			errors.push_back("Node must have array key 'regions'");
			return false;
		}

		VirtualNodeDescriptor node;
		node.id = json.get("id", 0).asUInt();
		node.name = json.get("name", "Generic Virtual Node").asString();
		
		const std::string& conceptRaw = json.get("concept", "haptic").asString();
		auto it = node_concept_map.find(conceptRaw);
		if (it == node_concept_map.end()) {
			errors.push_back("Concept '" + conceptRaw + "' was not recognized");
			return false;
		}

		node.concept = it->second;

		
		for (std::size_t i = 0; i < json["regions"].size(); i++) {
			if (!json["regions"][i].isString()) {
				errors.push_back("Region [" + std::to_string(i) + "] must be a string");
				return false;
			}

			node.regions.push_back(json["regions"][i].asString());
		}

		descriptors.push_back(node);

		return true;

	}
	bool deserialize(std::vector<VirtualDeviceDescriptor>& descriptors, const Json::Value& value, std::vector<std::string> & errors) {
		bool hasName = value["name"].isString();
		bool hasConcept = value["concept"].isString();
		bool hasNodes = value["nodes"].isArray();

		if (!hasName) {
			errors.push_back("Device must have string key 'name'");
			return false;
		}
		else if (!hasConcept) {
			errors.push_back("Device must have string key 'concept'");
			return false;
		}
		else if (!hasNodes) {
			errors.push_back("Device must have array key 'nodes'");
			return false;
		}

		VirtualDeviceDescriptor vd;
		vd.name = value.get("name", "Generic Virtual Device").asString();
		
		const std::string& conceptRaw = value.get("concept", "suit").asString();
		auto it = concept_map.find(conceptRaw);
		if (it == concept_map.end()) {
			errors.push_back("Concept '" + conceptRaw + "' was not recognized");
			return false;
		}

		vd.concept = it->second;
		
		const auto size = value["nodes"].size();
		for (std::size_t i = 0; i < size; i++) {
			const auto& node = value["nodes"][i];
			if (!deserialize(vd.nodes, node, errors)) {
				errors.insert(errors.begin(), "Error parsing node [" + std::to_string(i) + "]");
				return false;
			}
		}

		descriptors.push_back(vd);

		return true;



	}


	bool deserialize(VirtualDeviceList & descriptor, const Json::Value & json, std::vector<std::string> & errors)
	{
		const auto size = json.size();
		for (std::size_t i = 0; i < size; i++) {
			const auto& value = json[i];
			if (!deserialize(descriptor.devices, value, errors)) {
				errors.insert(errors.begin(), "Error parsing virtual device [" + std::to_string(i) + "]");
				return false;
			}
		}
		
		return true;
	}


	bool deserialize(ManifestDescriptor& descriptor, const Json::Value& json, std::vector<std::string>& errors)
	{
		//optional
		descriptor.version = json.get("manifest-version", 1).asUInt();



		descriptor.pluginName = json.get("name", "unknown").asString();
		if (descriptor.pluginName == "unknown") {
			errors.emplace_back("Must contain key 'name");
			return false;
		}

		if (!deserialize(descriptor.bodygraph, json["bodygraph"], errors)) {
			errors.insert(errors.begin(), "Error parsing BodyGraph");
			return false;
		}

		if (json["virtual_devices"].isArray()) {
			if (!deserialize(descriptor.vdevices, json["virtual_devices"], errors)) {
				errors.insert(errors.begin(), "Error parsing VirtualDevices");
				return false;
			}
		}


		return true;
	}


	ManifestDescriptor::ManifestDescriptor()
		: pluginName()
		, version(1)
		, bodygraph()
	{

	}

	BodyGraphDescriptor::BodyGraphDescriptor()
		: regions()
	{

	}

	MultiRegionDescriptor::MultiRegionDescriptor()
		: name()
		, bodypart(nsvr_bodypart_unknown)
		, location_start()
		, location_end()
		, count(0)
	{
	}

	SingleRegionDescriptor::SingleRegionDescriptor()
		: name()
		, bodypart(nsvr_bodypart_unknown)
		, location()
	{

	}

	LocationDescriptor::LocationDescriptor()
		: height(0)
		, rotation(0)
	{
	}


	
	boost::optional<Parsing::ManifestDescriptor> ParseConfig(const std::string & path)
	{
		try {
			Json::Value root;
			std::ifstream json(path, std::ifstream::binary);
			json >> root;

			Parsing::ManifestDescriptor manifest;
			std::vector<std::string> errors;

			if (!Parsing::deserialize(manifest, root, errors)) {
				std::string header("When parsing the manifest at " + path + ":");
				std::cout << header << '\n';
				
				int len = header.length();
				std::cout << std::string(len, '-') << '\n';
				
				int indent_level = 0;
				for (const auto& error : errors) {
					std::cout << std::string(indent_level * 4, ' ') << error << '\n';

					if (error.find("parsing") != std::string::npos) {
						indent_level++;
					}
				}

				std::cout << std::string(len, '-') << '\n';
				return boost::none;
			}
			else {
				return manifest;
			}

		}
		catch (const Json::Exception& e) {
			std::cout << "There was a json exception when parsing " << path << ": " << e.what() << '\n';
			return boost::none;
		}

	}

	

}



