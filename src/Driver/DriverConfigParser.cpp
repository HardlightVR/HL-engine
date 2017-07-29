#include "stdafx.h"
#include "DriverConfigParser.h"
#include <boost/filesystem.hpp>
#include <fstream>


std::unordered_map<std::string, NodeDescriptor::Capability> DriverConfigParser::capability_map = {
	{"preset", NodeDescriptor::Capability::Preset},
	{"buffered", NodeDescriptor::Capability::Buffered},
	{"dynamic", NodeDescriptor::Capability::Dynamic}
};

std::unordered_map<std::string, NodeDescriptor::NodeType> DriverConfigParser::node_type_map = {
	{"haptic", NodeDescriptor::NodeType::Haptic},
	{"tracked", NodeDescriptor::NodeType::Tracker},
	{"led", NodeDescriptor::NodeType::Led}
};

std::unordered_map<std::string, HardwareDescriptor::Concept> HardwareDescriptor::concept_map = {
	{"suit", HardwareDescriptor::Concept::Suit},
	{"controller", HardwareDescriptor::Concept::Controller},
	{"gun", HardwareDescriptor::Concept::Gun}
};

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


HardwareDescriptor DriverConfigParser::ParseConfig(const std::string & path)
{
	Json::Value root;
	std::ifstream json(path, std::ifstream::binary);
	json >> root;

	HardwareDescriptor descriptor;

	descriptor.fileVersion = root.get("manifest-version", 1).asUInt();
	std::string conceptStr = root.get("concept", "unknown").asString();
	descriptor.concept = HardwareDescriptor::concept_map[conceptStr];

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
	NodeDescriptor nodeDescriptor;

	assert(node.isObject());

	nodeDescriptor.displayName = node.get("name", "unknown").asString();
	nodeDescriptor.region = node.get("region", "unknown").asString();
	nodeDescriptor.nodeType = node_type_map[node.get("type", "unknown").asString()];
	const auto& capabilities = node["capabilities"];
	assert(capabilities.isArray());

	for (const Json::Value& cap : capabilities) {
		NodeDescriptor::Capability parsed = capability_map[cap.asString()];
		if (parsed != NodeDescriptor::Capability::Unknown) {
			nodeDescriptor.capabilities.insert(parsed);
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
