#include "stdafx.h"
#include "DriverConfigParser.h"
#include <boost/filesystem.hpp>
#include <fstream>
DriverConfigParser::DriverConfigParser()
{
}

bool DriverConfigParser::IsValidConfig(const std::string& path)
{
	Json::Value root;
	std::ifstream json(path, std::ifstream::binary);

	try {
		json >> root;
	}
	catch (const Json::Exception& e) {
		std::cout << "Error validating config: " <<  e.what() << '\n';
		return false;
	}

	return true;
	
}

HardwareDescriptor::Concept parseConcept(const std::string& conceptStr) {
	if (conceptStr == "suit") {
		return HardwareDescriptor::Concept::Suit;
	}
	else if (conceptStr == "gun") {
		return HardwareDescriptor::Concept::Gun;
	}
	else {
		return HardwareDescriptor::Concept::Unknown;
	}
}
HardwareDescriptor DriverConfigParser::ParseConfig(const std::string & path)
{
	Json::Value root;
	std::ifstream json(path, std::ifstream::binary);
	json >> root;

	HardwareDescriptor descriptor;

	descriptor.fileVersion = root.get("manifest-version", 1).asUInt();
	std::string conceptStr = root.get("concept", "unknown").asString();
	descriptor.concept = parseConcept(conceptStr);

	if (descriptor.concept == HardwareDescriptor::Concept::Unknown) {
		throw std::runtime_error("Unknown hardware concept: " + conceptStr);
	}

	descriptor.displayName = root.get("name", "unknown").asString();

	Json::Value nodes = root["nodes"];
	if (!nodes.isArray()) {
		throw std::runtime_error("You must have an array of nodes!");
	}

	auto size = nodes.size();
	for (Json::ArrayIndex i = 0; i < size; i++) {
		parseNodes(descriptor, nodes[i]);
	}

	return descriptor;
}

void DriverConfigParser::parseNodes(HardwareDescriptor& descriptor, const Json::Value& node)
{
	assert(node.isObject());

	std::string nodeName = node.get("name", "unknown").asString();
	std::string region = node.get("region", "unknown").asString();

	NodeDescriptor nodeDescriptor;
	nodeDescriptor.displayName = nodeName;
	nodeDescriptor.region = region;

	auto capabilities = node["capabilities"];

	assert(capabilities.isArray());

	//auto size = capabilities.size();

	for (const auto& cap : capabilities) {
		std::string interface = cap.get("interface", "unknown").asString();
		if (interface == "direct-drive") {
			nodeDescriptor.capabilities.push_back(direct_drive_interface{});
		}
		else if (interface == "brief-haptic") {
			nodeDescriptor.capabilities.push_back(brief_haptic_interface{});
		}
		else if (interface == "lasting-haptic") {
			nodeDescriptor.capabilities.push_back(lasting_haptic_interface{});
		}
	}

	descriptor.nodes.push_back(std::move(nodeDescriptor));


}

HardwareDescriptor::HardwareDescriptor()
{
}

NodeDescriptor::NodeDescriptor()
{
}
