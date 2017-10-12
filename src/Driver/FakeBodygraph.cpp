#include "stdafx.h"
#include "FakeBodygraph.h"

void FakeBodygraph::Augment(bodygraph_api* api) {
	api->submit_setup.handler = [](auto a, auto b) {};
}
//precondition: no duplicate nodes
FakeBodygraph::FakeBodygraph(std::vector<std::pair<nsvr_node_id, nsvr_region>> nodes )
{
	for (auto& pair : nodes) {
		m_nodeToRegion[pair.first].push_back(pair.second);
		m_regionToNode[pair.second].push_back(pair.first);
	}
}

void FakeBodygraph::ForEachNodeAtRegion(nsvr_region region, NodeAction action)
{
	for (nsvr_node_id id : m_regionToNode[region]) {
		action(id);
	}
}

std::vector<nsvr_node_id> FakeBodygraph::GetNodesAtRegion(nsvr_region region) const
{
	auto it = m_regionToNode.find(region);
	if (it != m_regionToNode.end()) {
		return it->second;
	}
	else {
		return std::vector<nsvr_node_id>{};
	}
}

std::vector<nsvr_node_id> FakeBodygraph::GetNodesAtRegions(const std::vector<nsvr_region>& region) const
{
	std::vector<nsvr_node_id> all_nodes;
	for (nsvr_region r : region) {
		auto result = GetNodesAtRegion(r);
		all_nodes.insert(all_nodes.begin(), result.begin(), result.end());
	}

	std::sort(all_nodes.begin(), all_nodes.end());
	all_nodes.erase(std::unique(all_nodes.begin(), all_nodes.end()), all_nodes.end());
	return all_nodes;
}

void FakeBodygraph::ForEachRegionPresent(std::function<void(nsvr_region, const std::vector<nsvr_node_id>&)> action)
{
	for (auto& kvp : m_regionToNode) {
		action(kvp.first, kvp.second);
	}
}

std::vector<nsvr_region> FakeBodygraph::GetRegionsForNode(nsvr_node_id node) const
{
	auto it = m_nodeToRegion.find(node);
	if (it != m_nodeToRegion.end()) {
		return it->second;
	}
	else {
		return std::vector<nsvr_region>{};
	}
}

FakeBodygraphBuilder::FakeBodygraphBuilder()
	: m_nodes()
{
}

FakeBodygraphBuilder & FakeBodygraphBuilder::WithNodeAtRegion(nsvr_node_id id, nsvr_region reg)
{
	m_nodes.push_back(std::make_pair(id, reg));
	return *this;
}

std::unique_ptr<FakeBodygraph> FakeBodygraphBuilder::Build()
{
	return std::make_unique<FakeBodygraph>(m_nodes);
}
