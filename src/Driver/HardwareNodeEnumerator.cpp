#include "stdafx.h"
#include "HardwareNodeEnumerator.h"
HardwareNodeEnumerator::HardwareNodeEnumerator(device_api * api)
	: m_api(api)
	, m_nodes()
{
}

void HardwareNodeEnumerator::setId(nsvr_device_id id)
{
	m_id = id;
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

void HardwareNodeEnumerator::ForEachNode(HardwareNodeEnumerator::NodeAction action)
{
	for (auto& kvp : m_nodes) {
		action(&kvp.second);
	}
}

std::vector<nsvr_node_id> HardwareNodeEnumerator::GetNodesOfType(nsvr_node_concept type)
{
	std::vector<nsvr_node_id> filteredNodes;
	for (auto& kvp : m_nodes) {
		if (kvp.second.type() == type) {
			filteredNodes.push_back(kvp.first);
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

std::vector<nsvr_node_id> HardwareNodeEnumerator::FilterByType(const std::vector<nsvr_node_id>& items, nsvr_node_concept type) const
{
	std::vector<nsvr_node_id> output;
	for (nsvr_node_id id : items) {
		auto it = m_nodes.find(id);
		if (it != m_nodes.end()) {
			if (it->second.type() == type) {
				output.push_back(it->first);
			}
		}
	}
	return output;
}



void HardwareNodeEnumerator::fetchNodeInfo(nsvr_node_id node_id)
{
	nsvr_node_info info{ {0}, nsvr_node_concept_unknown, nsvr_api_support_none};

	m_api->submit_getnodeinfo(node_id, &info);

	NodeDescriptor desc;
	desc.displayName = std::string(info.name);
	desc.id = node_id;
	desc.type = info.concept;
	desc.apiSupport = info.api_support;

	createNewNode(desc);
}

void HardwareNodeEnumerator::createNewNode(const NodeDescriptor& desc)
{
	m_nodes[desc.id] = Node(desc);

	
}

//So the current question is: how should nodes specify what theysupport?
/*
For instance the plugin supports the Waveform API. But how do we know if Node A supports it? 

Maybe Node A is a kicker, and only supports the Impact API, not the Waveform API. 

Should it be like:

Node {
	caps: supports_kicker | supports_buffered
	concept: haptic
	name: "Left shoulder kicker"


	hmm interesting. You could say "Hey, this LED actually supports waveform as well. Whoah dude. Therefore any haptic waveform sent to the 
	pads on this suit will also be sent to the LEDs on those pads."

	Okay so instead of saying, in the device, "If apis->supports waveform" we say something like "if (this particular node) supports waveform
*/