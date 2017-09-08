#include "stdafx.h"
#include "HardwareNodeEnumerator.h"

HardwareNodeEnumerator::HardwareNodeEnumerator(nsvr_device_id id, device_api * api)
	: m_api(api)
	, m_nodes()
	, m_id(id)
{
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

Node* HardwareNodeEnumerator::Get(nsvr_node_id id)
{
	if (m_nodes.find(id) != m_nodes.end()) {
		return &m_nodes.at(id);
	} 

	return nullptr;
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
	m_nodes.insert(std::make_pair(desc.id, Node(desc)));
}
