#include "stdafx.h"
#include "HardwareNodeEnumerator.h"

HardwareNodeEnumerator::HardwareNodeEnumerator(nsvr_device_id id, device_api * api)
	: m_api(api)
	, m_nodes()
	, m_id(id)
{
	Discover();
}

void HardwareNodeEnumerator::Discover()
{
	nsvr_node_ids device_ids = { 0 };
	m_api->submit_enumeratenodes(m_id, &device_ids);

	std::vector<NodeDescriptor> devices;
	devices.reserve(device_ids.node_count);

	for (std::size_t i = 0; i < device_ids.node_count; i++) {
		fetchNodeInfo(device_ids.ids[i]);
	}
}

void HardwareNodeEnumerator::ForEachNode(NodeDiscoverer::NodeAction action)
{
	for (auto& kvp : m_nodes) {
		action(&kvp.second);
	}
}

std::vector<Node*> HardwareNodeEnumerator::GetNodesOfType(nsvr_node_type type)
{
	std::vector<Node*> filteredNodes;
	for (auto& kvp : m_nodes) {
		if (kvp.second.type() == type) {
			filteredNodes.push_back(&m_nodes.at(kvp.first));
		}
	}

	return filteredNodes;

}

Node* HardwareNodeEnumerator::Get(nsvr_node_id id)
{
	if (m_nodes.find(id) != m_nodes.end()) {
		return &m_nodes.at(id);
	} 

	return nullptr;
}

std::vector<nsvr_node_id> HardwareNodeEnumerator::FilterByType(const std::vector<nsvr_node_id>& items, nsvr_node_type type)
{
	std::vector<nsvr_node_id> output;
	for (nsvr_node_id id : items) {
		auto it = m_nodes.find(id);
		if (it != m_nodes.end()) {
			output.push_back(it->first);
		}
	}
	return output;
}

void HardwareNodeEnumerator::fetchNodeInfo(nsvr_node_id node_id)
{
	nsvr_node_info info = { 0 };
	m_api->submit_getnodeinfo(node_id, &info);

	NodeDescriptor desc;
	desc.displayName = std::string(info.name);
	desc.id = node_id;
	desc.type = info.type;

	createNewNode(desc);
}

void HardwareNodeEnumerator::createNewNode(const NodeDescriptor& desc)
{
	m_nodes[desc.id] = Node(desc);
}
