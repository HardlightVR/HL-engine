#pragma once
#include <cstdint>
#include "PluginAPI.h"

//The point of these classes was to stop mistakes of leaking local ids to across shared memory.
//I have failed at this because the lower level classes just take uint32_ts, and don't have /need constructors.

//So maybe we should add constructors to the SharedMemory types, so that we can avoid this type of error.
//Ex: 

//SharedMemory::NodeInfo(GlobalNodeId id)


struct GlobalNodeId {
	uint32_t id;
};

struct LocalNodeId {
	nsvr_node_id id;
};

inline bool operator==(const GlobalNodeId& lhs, const GlobalNodeId& rhs) {
	return lhs.id == rhs.id;
}


namespace std {
	template<> struct hash<GlobalNodeId> {
		size_t operator()(const GlobalNodeId& x) const {
			return hash<uint32_t>{}(x.id);
		}
	};

	template<> struct hash<LocalNodeId> {
		size_t operator()(const LocalNodeId& x) const {
			return hash<uint32_t>{}(x.id);
		}
	};
}