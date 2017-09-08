#pragma once
#include "NodeDiscoverer.h"
#include "PluginApis.h"
#include <unordered_map>


class HardwareNodeEnumerator : public NodeDiscoverer {

public:
	HardwareNodeEnumerator(nsvr_device_id device_id, device_api* api);
	void Discover() override;
	void ForEachNode(NodeDiscoverer::NodeAction action) override;
	Node* Get(nsvr_node_id id) override;
private:
	nsvr_device_id m_id;
	std::unordered_map<nsvr_node_id, Node> m_nodes;
	device_api* m_api;

	void fetchNodeInfo(nsvr_node_id id);
	void createNewNode(const NodeDescriptor& desc);
};