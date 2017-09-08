#pragma once

#include <functional>
#include "PluginAPI.h"
#include "DriverConfigParser.h"
#include "PluginApis.h"
#include "nsvr_region.h"
class BodyGraphCreator {
public:
	using NodeAction = std::function<void(nsvr_node_id)>;
	virtual ~BodyGraphCreator() {};
	
	virtual void ForEachNodeAtRegion(nsvr_region region, NodeAction action) = 0;
	virtual std::vector<nsvr_node_id> GetNodesAtRegion(nsvr_region region) const = 0;
};