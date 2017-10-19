#pragma once
#include <vector>
#include <memory>
#include "PluginApis.h"
#include "Node.h"

class FakeDiscoverer {
public:
	FakeDiscoverer(std::vector<Node> nodes = {});
	
	void Augment(device_api* device_api);
private:
	void get_info(nsvr_node_id id, nsvr_node_info* info);
	void enumerate(nsvr_node_ids* nodes);
	std::unordered_map<NodeId<local>, Node> m_nodes;
};



class FakeDiscovererBuilder {
public:
	FakeDiscovererBuilder();
	FakeDiscovererBuilder& WithNode(nsvr_node_id id, nsvr_node_concept type, std::string name);
	std::unique_ptr<FakeDiscoverer> Build();
private:
	std::vector<Node> m_nodes;
};