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
	struct Color {
		float r;
		float g;
		float b;
		float a;
	};

	struct Intensity {
		float intensity;
		uint32_t category;
	};

	struct Rotation {
		float w;
		float x;
		float y;
		float z;
	};
	using Data = boost::variant<Color, Intensity, Rotation>;
	using SingleNode = std::pair<NodeType, Data>;
	uint64_t region;
	std::vector<SingleNode> nodes;
};