#pragma once

#include "IHardwareDevice.h"

#include "PluginAPI.h"
#include <functional>
class NodeDiscoverer {
public:
	using NodeAction = std::function<void(Node*)>;

	virtual ~NodeDiscoverer() {}
	virtual void Discover() = 0;
	virtual Node* Get(nsvr_node_id id) = 0;
	virtual void ForEachNode(NodeAction action) = 0;

};