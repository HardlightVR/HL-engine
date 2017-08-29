#include "stdafx.h"
#include "DriverConfigParser.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include "Locator.h"

std::unordered_map<std::string, NodeDescriptor::Capability> DriverConfigParser::capability_map = {
	{"preset", NodeDescriptor::Capability::Preset},
	{"buffered", NodeDescriptor::Capability::Buffered},
	{"dynamic", NodeDescriptor::Capability::Dynamic}
};

std::unordered_map<std::string, NodeDescriptor::NodeType> DriverConfigParser::node_type_map = {
	{"haptic", NodeDescriptor::NodeType::Haptic},
	{"led", NodeDescriptor::NodeType::Led},
	{"tracker", NodeDescriptor::NodeType::Tracker}
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
	const std::string& conceptStr = root.get("concept", "unknown").asString();
	descriptor.concept = HardwareDescriptor::concept_map[conceptStr];

	descriptor.displayName = root.get("name", "unknown").asString();

	Json::Value devices = root["devices"];
	if (!devices.isArray()) {
		throw std::runtime_error("You must have an array of devices!");
	}

	auto size = devices.size();
	for (Json::ArrayIndex i = 0; i < size; i++) {
		parseDevice(descriptor, devices[i]);
	}

	return descriptor;
}

void DriverConfigParser::parseDevice(HardwareDescriptor& descriptor, const Json::Value& node)
{
	DeviceDescriptor nodeDescriptor;

	assert(node.isObject());

	nodeDescriptor.displayName = node.get("name", "unknown").asString();

	nodeDescriptor.id = node.get("id", 0).asUInt();
	descriptor.devices.push_back(std::move(nodeDescriptor));


}



HardwareDescriptor::HardwareDescriptor()
{
}

DeviceDescriptor::DeviceDescriptor()
{
}
