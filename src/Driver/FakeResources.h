#pragma once

#include "PluginApis.h"
#include "logger.h"
#include "Node.h"
#include <unordered_map>
#include <chrono>
#include "ScheduledEvent.h"
#include "runtime_include/NSDriverApi.h"


template<typename Api>
struct FakeInterface {
	virtual void Augment(Api* api) {}
};


struct DefaultWaveform : public FakeInterface<waveform_api> {
	void Augment(waveform_api* api) override {
		api->submit_activate.handler = [](auto...){};
	}
};

struct DefaultBuffered : public FakeInterface<buffered_api> {
	void Augment(buffered_api* api) override {
		api->submit_buffer.handler = [](auto...) {};
		api->submit_getsampleduration.handler = [](nsvr_node_id, double* outDuration, void* ud) { *outDuration = 20; };
	}
};


struct DefaultBodygraph : public FakeInterface<bodygraph_api> {

	struct association {
		std::string node;
		nsvr_node_id id;
	};
	DefaultBodygraph(std::vector<association> assocs = {}) : m_assocs(assocs) {}
	
	void Augment(bodygraph_api* api) override {
		api->submit_setup.cpp_fn = [this](nsvr_bodygraph* bg) {
			for (const auto& assoc : m_assocs) {
				nsvr_bodygraph_associate(bg, assoc.node.c_str(), assoc.id);
			}
		};
	}

	std::vector<association> m_assocs;

};
struct DefaultTracking : public FakeInterface<tracking_api> {
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
		api->submit_beginstreaming.cpp_fn = [this](nsvr_tracking_stream* stream, nsvr_node_id id) {
			m_streams[id] = stream;
			m_beginningOfTime = std::chrono::high_resolution_clock::now();
			m_timers[id]->Start();
		};

		api->submit_endstreaming.cpp_fn = [this](nsvr_node_id id) {
			m_timers[id]->Stop();
		};
	}

	void write_tracking(nsvr_node_id id) {
		if (m_dataProvider) {
			hvr_quaternion quat = { 0 };
			m_dataProvider(id, &quat);
			nsvr_quaternion dupe{ quat.w, quat.x, quat.y, quat.z };
			nsvr_tracking_stream_push_quaternion(m_streams[id], &dupe);
		}
	}
	
	std::unordered_map<nsvr_node_id, nsvr_tracking_stream*> m_streams;
	std::unordered_map<nsvr_node_id, std::unique_ptr<ScheduledEvent>> m_timers;
	std::chrono::high_resolution_clock::time_point m_beginningOfTime;
};



struct DefaultNodeDiscoverer : public FakeInterface<device_api> {
	DefaultNodeDiscoverer(const std::vector<Node>& nodes = {}) {
		for (const auto& node : nodes) {
			m_nodes[node.id()] = node;
		}
	}
	void Augment(device_api* api) override {

		api->submit_enumeratenodes.cpp_fn = [this](nsvr_device_id id, nsvr_node_ids* ids) {
			std::size_t index = 0;
			for (const auto& kvp : m_nodes) {
				ids->ids[index] = kvp.first.value;
				index++;
			}

			ids->node_count = m_nodes.size();
		};

		api->submit_getnodeinfo.cpp_fn = [this](nsvr_device_id did, nsvr_node_id id, nsvr_node_info* info) {
			auto node = m_nodes.at(NodeId<local>{id});
			std::string name = node.name();
			std::copy(name.begin(), name.end(), info->name);
			info->concept = node.type();
		};
	}


	std::unordered_map<NodeId<local>, Node> m_nodes;


};
