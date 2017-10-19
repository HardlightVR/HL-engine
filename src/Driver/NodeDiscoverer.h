#pragma once


#include "PluginAPI.h"
#include <functional>
#include <vector>
#include "Node.h"


//todo: delete this class. It doesn't need to be an interface. 
class NodeDiscoverer {
public:
	using NodeAction = std::function<void(Node*)>;

	virtual ~NodeDiscoverer() = default;
	virtual void Discover() = 0;
	virtual Node* Get(nsvr_node_id id) = 0;
	virtual void ForEachNode(NodeAction action) = 0;
	virtual std::vector<nsvr_node_id> GetNodesOfType(nsvr_node_concept type) = 0;
	virtual std::vector<nsvr_node_id> FilterByType(const std::vector<nsvr_node_id>& items, nsvr_node_concept type) const = 0;
	virtual std::vector<nsvr_node_id> FilterBySupport(const std::vector<nsvr_node_id>& items, nsvr_api_support type) const = 0;


};