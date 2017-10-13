#pragma once

#include "PluginApis.h"
#include "logger.h"
#include "Node.h"
#include <unordered_map>
#define DECLARE_FAKE_INTERFACE(name, api) \
struct name { \
virtual ~##name##() = default; \
virtual void Augment(##api##*) {} \
}; 


DECLARE_FAKE_INTERFACE(FakeWaveformHaptics, waveform_api)
DECLARE_FAKE_INTERFACE(FakeBufferedHaptics, buffered_api)
DECLARE_FAKE_INTERFACE(FakeNodeDiscoverer, device_api)
DECLARE_FAKE_INTERFACE(FakeBodygraph, bodygraph_api)
DECLARE_FAKE_INTERFACE(FakePlayback, playback_api)
DECLARE_FAKE_INTERFACE(FakeTracking, tracking_api)


struct NullNodeDiscoverer : public FakeNodeDiscoverer {
	NullNodeDiscoverer(const std::vector<Node>& nodes = {}) {
		for (const auto& node : nodes) {
			m_nodes[node.id()] = node;
		}
	}
	void Augment(device_api* api) override {

		api->submit_enumeratenodes.handler = [](nsvr_device_id id, nsvr_node_ids* ids, void* ud) {
			static_cast<NullNodeDiscoverer*>(ud)->enumerate(ids);

		};
		api->submit_enumeratenodes.user_data = this;

		api->submit_getnodeinfo.handler = [](nsvr_node_id id, nsvr_node_info* info, void* ud) {
			static_cast<NullNodeDiscoverer*>(ud)->info(id, info);
		};
		api->submit_getnodeinfo.user_data = this;
	}

	void enumerate(nsvr_node_ids* ids) {
		std::size_t index = 0;
		for (const auto& kvp : m_nodes) {
			ids->ids[index] = kvp.first.value;
			index++;
		}

		ids->node_count = m_nodes.size();
	}
	std::unordered_map<NodeId<local>, Node> m_nodes;

	void info(nsvr_node_id id, nsvr_node_info* info)
	{
		auto node = m_nodes.at(NodeId<local>{id});
		info->id = id;
		std::string name = node.name();
		std::copy(name.begin(), name.end(), info->name);
		info->type = node.type();
	}


};
