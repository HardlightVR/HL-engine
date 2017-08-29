#pragma once

#include <string>
#include <vector>

#include <boost/variant.hpp>
#include <unordered_set>

class DeviceDescriptor {
public:
	DeviceDescriptor();
	uint32_t id;
	std::string displayName;

};

class NodeDescriptor {
public:
	enum class NodeType {
		Unknown = 0,
		Haptic,
		Led,
		Tracker
	};
	enum class Capability {
		Unknown = 0,
		Preset = 1 << 0,
		Buffered = 1 << 1,
		Dynamic = 1 << 2
	};

	uint64_t id;
	std::string displayName;
	NodeType type;
	uint32_t capabilities;
};

class HardwareDescriptor {
public:
	enum class Concept {
		Unknown = 0,
		Suit,
		Gun,
		Controller
	};
	HardwareDescriptor();

	std::string displayName;
	Concept concept;
	std::vector<DeviceDescriptor> devices;
	unsigned int fileVersion;
	static std::unordered_map<std::string, HardwareDescriptor::Concept> concept_map;

};
class DriverConfigParser {

public:
	DriverConfigParser();
	static bool IsValidConfig(const std::string& path);
	static HardwareDescriptor ParseConfig(const std::string& path);


	static void parseDevice(HardwareDescriptor& descriptor, const Json::Value& devices);
	//static DeviceDescriptor::Capability parseCapability(const std::string& param1);
	//static DeviceDescriptor::NodeType parseNodeType(const std::string& param1);


	

	static std::unordered_map<std::string, NodeDescriptor::Capability> capability_map;
	static std::unordered_map<std::string, NodeDescriptor::NodeType> node_type_map;

};