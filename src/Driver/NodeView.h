#pragma once
#include <stdint.h>
#include <boost/variant.hpp>
#include <vector>

struct NodeView {
	enum class NodeType : uint64_t {
		Unknown = 0,
		Haptic = 1,
		Led = 2,
		Tracking = 3
	};
	struct Data {
		float data_0;
		float data_1;
		float data_2;
		float intensity;
	};
	
	using SingleNode = std::pair<NodeType, Data>;
	uint64_t region;
	uint64_t id;

	std::vector<SingleNode> nodes;
};