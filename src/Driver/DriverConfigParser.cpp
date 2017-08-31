#include "stdafx.h"
#include "DriverConfigParser.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include "Locator.h"
namespace Parsing {
	static std::unordered_map<std::string, Concept> concept_map = {
		{ "suit", Concept::Suit },
		{ "controller", Concept::Controller },
		{ "gun", Concept::Gun }
	};

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

	bool deserialize(SingleRegionDescriptor& descriptor, const Json::Value& json, std::vector<std::string>& errors){
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
		nsvr_bodypart parsedBodypart = translator.ToBodypart(rawBodyPart, nsvr_bodypart_unknown);
		if (parsedBodypart == nsvr_bodypart_unknown) {
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
		nsvr_bodypart parsedBodypart = translator.ToBodypart(rawBodyPart, nsvr_bodypart_unknown);
		if (parsedBodypart == nsvr_bodypart_unknown) {
			errors.push_back("Unrecognized bodypart: '" + rawBodyPart + "'");
			return false;
		}

		descriptor.count = json.get("count", 1).asUInt();


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

		for (std::size_t i = 0; i < json["regions"].size(); i++){
			const auto& value = json["regions"][i];
			if (!deserialize(descriptor.regions, value, errors)) {
				errors.insert(errors.begin(), "Error parsing region [" + std::to_string(i) + "]");
				return false;
			}
			

		}

		return true;

	}

	bool deserialize(ManifestDescriptor& descriptor, const Json::Value& json, std::vector<std::string>& errors)
	{
		//optional
		descriptor.version = json.get("manifest-version", 1).asUInt();

		std::string rawConcept = json.get("concept", "unknown").asString();
		if (rawConcept == "unknown") {
			errors.push_back("Must contain key 'concept'");
			return false;
		}

		if (concept_map.find(rawConcept) != concept_map.end()) {
			descriptor.concept = concept_map[rawConcept];
		}
		else {
			errors.push_back("Unknown concept: '" + rawConcept + "'");
			return false;
		}

		descriptor.pluginName = json.get("name", "unknown").asString();
		if (descriptor.pluginName == "unknown") {
			errors.push_back("Must contain key 'name");
			return false;
		}

		if (!deserialize(descriptor.bodygraph, json["bodygraph"], errors)) {
			errors.insert(errors.begin(), "Error parsing BodyGraph");
			return false;
		}

		return true;
	}

	ManifestDescriptor::ManifestDescriptor()
		: pluginName()
		, version(1)
		, concept(Parsing::Concept::Unknown)
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



