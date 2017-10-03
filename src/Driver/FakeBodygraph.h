#pragma once
#include "BodyGraphCreator.h"
#include <vector>


class FakeBodygraph : public BodyGraphCreator {
public:
	FakeBodygraph(std::vector<std::pair<nsvr_node_id, nsvr_region>>);
	void ForEachNodeAtRegion(nsvr_region region, NodeAction action) override;

	std::vector<nsvr_node_id> GetNodesAtRegion(nsvr_region region) const override;

	std::vector<nsvr_node_id> GetNodesAtRegions(const std::vector<nsvr_region>& region) const override;

	void ForEachRegionPresent(std::function<void(nsvr_region, const std::vector<nsvr_node_id>&)> action) override;

	std::vector<nsvr_region> GetRegionsForNode(nsvr_node_id node) const override;
private:
	std::unordered_map<nsvr_region, std::vector<nsvr_node_id>> m_regionToNode;
	std::unordered_map<nsvr_node_id, std::vector<nsvr_region>> m_nodeToRegion;
};

class FakeBodygraphBuilder {
public:
	FakeBodygraphBuilder();
	FakeBodygraphBuilder& WithNodeAtRegion(nsvr_node_id, nsvr_region);
	std::unique_ptr<FakeBodygraph> Build();
private:
	std::vector<std::pair<nsvr_node_id, nsvr_region>> m_nodes;
};