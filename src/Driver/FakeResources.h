#pragma once

#include "PluginApis.h"
#include "logger.h"
#include "Node.h"
#include <unordered_map>
#include <chrono>
#include "ScheduledEvent.h"
#include "runtime_include/NSDriverApi.h"
#define DECLARE_FAKE_INTERFACE(name, api) \
struct name { \
virtual ~##name##() {}; \
virtual void Augment(##api##*) {} \
}; 


DECLARE_FAKE_INTERFACE(FakeWaveformHaptics, waveform_api)
DECLARE_FAKE_INTERFACE(FakeBufferedHaptics, buffered_api)
DECLARE_FAKE_INTERFACE(FakeNodeDiscoverer, device_api)
DECLARE_FAKE_INTERFACE(FakeBodygraph, bodygraph_api)
DECLARE_FAKE_INTERFACE(FakePlayback, playback_api)
DECLARE_FAKE_INTERFACE(FakeTracking, tracking_api)


struct DefaultBodygraph : public FakeBodygraph {

	struct association {
		std::string node;
		nsvr_node_id id;
	};
	DefaultBodygraph(std::vector<association> assocs = {}) : m_assocs(assocs) {}
	void Augment(bodygraph_api* api) override {
		api->submit_setup.user_data = this;
		api->submit_setup.handler = [](nsvr_bodygraph* bg, void* ud) {
			static_cast<DefaultBodygraph*>(ud)->setup(bg);
		};
	}

	void setup(nsvr_bodygraph* bg) {
		for (const auto& assoc : m_assocs) {
			nsvr_bodygraph_associate(bg, assoc.node.c_str(), assoc.id);
		}
	}

	std::vector<association> m_assocs;

};
struct DefaultTracking : public FakeTracking {
	DefaultTracking() : m_timers() {}
	DefaultTracking(boost::asio::io_service& io, std::vector<nsvr_node_id> tracked_nodes = {}) : m_timers(), m_streams() {
		for (nsvr_node_id id : tracked_nodes) {
			auto e = std::make_unique<ScheduledEvent>(io, boost::posix_time::millisec(16));
			e->SetEvent([this, id]() {
				write_tracking(id);
			});
			m_timers.emplace(std::make_pair(id, std::move(e)));
		}
	}
	std::function<void(nsvr_node_id, hvr_quaternion*)> m_dataProvider;
	void SetCallback(std::function<void(nsvr_node_id, hvr_quaternion*)> cb) {
		m_dataProvider = cb;
	}
	void Augment(tracking_api* api) override {
		api->submit_beginstreaming.user_data = this;
		api->submit_beginstreaming.handler = [](nsvr_tracking_stream* stream, nsvr_node_id id, void* ud) {
			static_cast<DefaultTracking*>(ud)->begin(stream, id);
		};


		api->submit_endstreaming.user_data = this;
		api->submit_endstreaming.handler = [](nsvr_node_id id, void* ud) {
			static_cast<DefaultTracking*>(ud)->end(id);
		};
	}

	void write_tracking(nsvr_node_id id) {
		if (m_dataProvider) {
			hvr_quaternion quat = { 0 };
			m_dataProvider(id, &quat);
			nsvr_quaternion dupe{ quat.w, quat.x, quat.y, quat.z };
			nsvr_tracking_stream_push(m_streams[id], &dupe);
		}
	}
	
	void begin(nsvr_tracking_stream* stream, nsvr_node_id id) {
		m_streams[id] = stream;
		m_beginningOfTime = std::chrono::high_resolution_clock::now();
		m_timers[id]->Start();
	}
	void end(nsvr_node_id id) {
		m_timers[id]->Stop();
	}
	std::unordered_map<nsvr_node_id, nsvr_tracking_stream*> m_streams;
	std::unordered_map<nsvr_node_id, std::unique_ptr<ScheduledEvent>> m_timers;
	std::chrono::high_resolution_clock::time_point m_beginningOfTime;
};

struct DefaultNodeDiscoverer : public FakeNodeDiscoverer {
	DefaultNodeDiscoverer(const std::vector<Node>& nodes = {}) {
		for (const auto& node : nodes) {
			m_nodes[node.id()] = node;
		}
	}
	void Augment(device_api* api) override {

		api->submit_enumeratenodes.handler = [](nsvr_device_id id, nsvr_node_ids* ids, void* ud) {
			static_cast<DefaultNodeDiscoverer*>(ud)->enumerate(ids);

		};
		api->submit_enumeratenodes.user_data = this;

		api->submit_getnodeinfo.handler = [](nsvr_node_id id, nsvr_node_info* info, void* ud) {
			static_cast<DefaultNodeDiscoverer*>(ud)->info(id, info);
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
