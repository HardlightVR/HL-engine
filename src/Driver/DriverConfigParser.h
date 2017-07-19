#pragma once

#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <unordered_set>

struct brief_haptic_interface {
	
};

struct lasting_haptic_interface {
};

struct direct_drive_interface {
};

class NodeDescriptor {
public:
	using Capability = boost::variant<
		brief_haptic_interface,
		lasting_haptic_interface,
		direct_drive_interface
	>;

	NodeDescriptor();

	std::string displayName;
	std::string region;
	std::vector<Capability> capabilities;
};

class HardwareDescriptor {
public:
	enum class Concept {
		Unknown = 0,
		Suit,
		Gun
	};
	HardwareDescriptor();

	std::string displayName;
	Concept concept;
	std::vector<NodeDescriptor> nodes;
	unsigned int fileVersion;
};
class DriverConfigParser {

public:
	DriverConfigParser();
	static bool IsValidConfig(const std::string& path);
	static HardwareDescriptor ParseConfig(const std::string& path);




private:
	static void parseNodes(HardwareDescriptor& descriptor, const Json::Value& nodes);
};