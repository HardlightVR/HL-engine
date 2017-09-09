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

	//Returns a unique list of nodes at a given region
	virtual std::vector<nsvr_node_id> GetNodesAtRegion(nsvr_region region) const = 0;

	//Returns a set of nodes for a given list of regions
	virtual std::vector<nsvr_node_id> GetNodesAtRegions(const std::vector<nsvr_region>& region) const = 0;

	virtual void ForEachRegionPresent(std::function<void(nsvr_region, const std::vector<nsvr_node_id>&)> action) = 0;
};