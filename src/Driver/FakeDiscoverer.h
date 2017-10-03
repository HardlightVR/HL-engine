#pragma once
#include "NodeDiscoverer.h"
#include <vector>
#include <memory>

class FakeDiscoverer : public NodeDiscoverer {
public:
	FakeDiscoverer(std::vector<Node> nodes);
	void Discover() override;
	Node* Get(nsvr_node_id id) override;
	void ForEachNode(NodeAction action) override;
	std::vector<nsvr_node_id> GetNodesOfType(nsvr_node_type type) override;
	std::vector<nsvr_node_id> FilterByType(const std::vector<nsvr_node_id>& items, nsvr_node_type type) override;
private:
	std::unordered_map<NodeId<local>, Node> m_nodes;
};



class FakeDiscovererBuilder {
public:
	FakeDiscovererBuilder();
	FakeDiscovererBuilder& WithNode(nsvr_node_id id, nsvr_node_type type, std::string name);
	std::unique_ptr<FakeDiscoverer> Build();
private:
	std::vector<Node> m_nodes;
};