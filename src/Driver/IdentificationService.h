#include "PluginAPI.h"
#include <string>
#include <unordered_map>
#include <boost/optional.hpp>
#include <boost/bimap.hpp>

struct LocalDevice {
	uint32_t id;
	std::string plugin;
};
struct LocalNode {
	uint32_t id;
	std::string plugin;
	uint32_t device_id;
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
	using LocalDeviceId = nsvr_device_id;
	using GlobalDeviceId = uint32_t;
	using LocalNodeId = nsvr_node_id;
	using GlobalNodeId = uint32_t;
	GlobalDeviceId FromLocal(PluginId, LocalDeviceId);
	
	boost::optional<LocalDevice> FromGlobalDevice(GlobalDeviceId) const;


	GlobalNodeId FromLocal(PluginId, LocalDeviceId, LocalNodeId);

	boost::optional<LocalNode> FromGlobalNode(GlobalNodeId) const;

private:

	std::unordered_map<GlobalDeviceId, LocalDevice> deviceGlobalToLocal;
	std::unordered_map<LocalDevice, GlobalDeviceId> deviceLocalToGlobal;

	std::unordered_map<GlobalNodeId, LocalNode> nodeGlobalToLocal;
	std::unordered_map<LocalNode, GlobalNodeId> nodeLocalToGlobal;

	uint32_t currentGlobalDeviceId;
	uint32_t currentGlobalNodeId;

	uint32_t nextGlobalDeviceId();
	uint32_t nextGlobalNodeId();
};