#pragma once
#include <stdint.h>
#include "PluginAPI.h"
#include <vector>
#include "DeviceIds.h"

struct RenderedNode {

	struct GenericData {
		float data_0;
		float data_1;
		float data_2;
		float data_3;
	};
	NodeId<local> Id;
	nsvr_node_concept type;
	GenericData data;
};

struct RenderedRegion {
	uint32_t region;
	std::vector<RenderedNode> nodes;
};

