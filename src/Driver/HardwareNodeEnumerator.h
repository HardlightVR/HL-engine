#pragma once
#include "NodeDiscoverer.h"
#include "PluginApis.h"
#include <unordered_map>
#include "Node.h"
class DriverMessenger;

class HardwareNodeEnumerator : public NodeDiscoverer {

public:
	HardwareNodeEnumerator(device_api* api);
	void setId(nsvr_device_id id);
	void Discover() override;
	void ForEachNode(NodeDiscoverer::NodeAction action) override;
	std::vector<nsvr_node_id> GetNodesOfType(nsvr_node_type type) override;
	Node* Get(nsvr_node_id id) override;
	std::vector<nsvr_node_id> FilterByType(const std::vector<nsvr_node_id>& items, nsvr_node_type type) override;
private:
	nsvr_device_id m_id;
	std::unordered_map<nsvr_node_id, Node> m_nodes;
	device_api* m_api;
	void fetchNodeInfo(nsvr_node_id id);
	void createNewNode(const NodeDescriptor& desc);
};