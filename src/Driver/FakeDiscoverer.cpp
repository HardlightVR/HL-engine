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

void FakeDiscoverer::Augment(device_api * device_api)
{
	device_api->submit_enumeratenodes.handler = [](nsvr_device_id ignored, nsvr_node_ids* devices, void* ud) {
		static_cast<FakeDiscoverer*>(ud)->enumerate(devices);
	};
	device_api->submit_enumeratenodes.user_data = this;

	device_api->submit_getnodeinfo.handler = [](nsvr_node_id id, nsvr_node_info* info, void* ud) {
		static_cast<FakeDiscoverer*>(ud)->get_info(id, info);
	};
	device_api->submit_getnodeinfo.user_data = this;
}

void FakeDiscoverer::get_info(nsvr_node_id id, nsvr_node_info * info)
{
	info->id = id;
	info->type = m_nodes.at(NodeId<local>{id}).type();

	const std::string& name = m_nodes.at(NodeId<local>{id}).name();
	std::copy(name.begin(), name.end(), info->name);

}

void FakeDiscoverer::enumerate(nsvr_node_ids* nodes)
{
	std::size_t index = 0;
	for (const auto& kvp : m_nodes) {
		nodes->ids[index] = kvp.first.value;
		index++;
	}

	nodes->node_count = m_nodes.size();
}
