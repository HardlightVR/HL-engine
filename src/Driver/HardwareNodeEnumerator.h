#pragma once
#include "PluginApis.h"
#include <unordered_map>
#include "Node.h"
class DriverMessenger;

class HardwareNodeEnumerator {

public:
	using NodeAction = std::function<void(Node*)>;
	HardwareNodeEnumerator(device_api* api);
	void setId(nsvr_device_id id);
	void Discover();
	void ForEachNode(NodeAction action);
	std::vector<nsvr_node_id> GetNodesOfType(nsvr_node_concept type);
	Node* Get(nsvr_node_id id);
	std::vector<nsvr_node_id> FilterByType(const std::vector<nsvr_node_id>& items, nsvr_node_concept type) const;

	std::vector<nsvr_node_id> FilterBySupport(const std::vector<nsvr_node_id>& items, nsvr_api_support apis) const;
private:
	nsvr_device_id m_id;
	std::unordered_map<nsvr_node_id, Node> m_nodes;
	device_api* m_api;
	void fetchNodeInfo(nsvr_node_id id);
	void createNewNode(const NodeDescriptor& desc);
};