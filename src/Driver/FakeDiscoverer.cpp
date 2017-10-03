#include "stdafx.h"
#include "FakeDiscoverer.h"

FakeDiscovererBuilder::FakeDiscovererBuilder()
	: m_nodes{}
{
}

FakeDiscovererBuilder & FakeDiscovererBuilder::WithNode(nsvr_node_id id, nsvr_node_type type, std::string name)
{
	NodeDescriptor desc{ type, std::move(name), id };
	m_nodes.emplace_back(std::move(desc));

	return *this;
}

std::unique_ptr<FakeDiscoverer> FakeDiscovererBuilder::Build()
{
	return std::make_unique<FakeDiscoverer>(m_nodes);
}

FakeDiscoverer::FakeDiscoverer(std::vector<Node> nodes)
{
	for (Node& node : nodes) {
		m_nodes[node.id()] = node;
	}
}

void FakeDiscoverer::Discover()
{
	//no-op, because of our constructor
}

Node * FakeDiscoverer::Get(nsvr_node_id id)
{
	auto iter = m_nodes.find(NodeId<local>{id});
	if (iter != m_nodes.end()) {
		return &m_nodes.at(iter->first);
	}
	return nullptr;
}

void FakeDiscoverer::ForEachNode(NodeAction action)
{
	for (auto& kvp : m_nodes) {
		action(&kvp.second);
	}
}

std::vector<nsvr_node_id> FakeDiscoverer::GetNodesOfType(nsvr_node_type type)
{
	std::vector<nsvr_node_id> filtered;
	for (const auto& kvp : m_nodes) {
		if (kvp.second.type() == type) {
			filtered.push_back(kvp.first.value);
		}
	}
	return filtered;
}

std::vector<nsvr_node_id> FakeDiscoverer::FilterByType(const std::vector<nsvr_node_id>& items, nsvr_node_type type)
{
	std::vector<nsvr_node_id> filtered;
	for (nsvr_node_id id: items) {
		if (auto ptr = Get(id)) {
			if (ptr->type() == type) {
				filtered.push_back(id);
			}
		}
	}
	return filtered;
}
