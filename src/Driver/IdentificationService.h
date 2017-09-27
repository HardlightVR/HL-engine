#pragma once
#include "PluginAPI.h"
#include <string>
#include <unordered_map>
#include <boost/optional.hpp>
#include <boost/bimap.hpp>
#include "DeviceIds.h"

struct LocalDevice {
	nsvr_device_id id;
	std::string plugin;
};
struct LocalNode {
	nsvr_node_id id;
	std::string plugin;
	nsvr_device_id device_id;
};


bool operator==(const LocalNode& lhs, const LocalNode& rhs);

bool operator==(const LocalDevice& lhs, const LocalDevice& rhs);
namespace std {
	template<> struct hash<LocalDevice> {
		size_t operator()(const LocalDevice& x) const {
			std::size_t seed = 0;
			boost::hash_combine(seed, x.id);
			boost::hash_combine(seed, x.plugin);
			return seed;
		}
	};

	template<> struct hash<LocalNode> {
		size_t operator()(const LocalNode& x) const {
			std::size_t seed = 0;
			boost::hash_combine(seed, x.id);
			boost::hash_combine(seed, x.device_id);
			boost::hash_combine(seed, x.plugin);
			return seed;
		}
	};

	
}

class IdentificationService {

public:
	
	IdentificationService();
	using PluginId = std::string;


	DeviceId<global> FromLocal(PluginId pluginName, DeviceId<local>);
	
	boost::optional<LocalDevice> FromGlobalDevice(DeviceId<global>) const;


	NodeId<global> FromLocal(PluginId pluginName, DeviceId<local>, NodeId<local>);

	boost::optional<LocalNode> FromGlobalNode(NodeId<global>) const;

private:

	std::unordered_map<DeviceId<global>, LocalDevice> deviceGlobalToLocal;
	std::unordered_map<LocalDevice, DeviceId<global>> deviceLocalToGlobal;

	std::unordered_map<NodeId<global>, LocalNode> nodeGlobalToLocal;
	std::unordered_map<LocalNode, NodeId<global>> nodeLocalToGlobal;

	uint32_t currentGlobalDeviceId;
	uint32_t currentGlobalNodeId;

	DeviceId<global> nextGlobalDeviceId();
	NodeId<global> nextGlobalNodeId();
};