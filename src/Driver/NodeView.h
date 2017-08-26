#pragma once
#include <stdint.h>
#include <boost/variant.hpp>
#include <vector>

struct NodeView {
	enum class NodeType : uint32_t {
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
	
	struct SingleNode {
		NodeType Type;
		uint64_t Id;
		Data DisplayData;

	};
	uint32_t region;

	std::vector<SingleNode> nodes;
};