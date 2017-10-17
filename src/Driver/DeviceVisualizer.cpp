#include "stdafx.h"
#include "DeviceVisualizer.h"
#include "nsvr_preset.h"
#include "DeviceIds.h"

void DeviceVisualizer::provideApi(waveform_api* waveform)
{
	waveform->submit_activate.instrumentation = [this](uint64_t request_id, nsvr_node_id id, nsvr_waveform* waveform) {
		Waveform w = Waveform(request_id, waveform->waveform_id, waveform->strength, waveform->repetitions);
		m_nodes[id].submitHaptic(w);
	};
}

void DeviceVisualizer::provideApi(playback_api* playback)
{
	playback->submit_cancel.instrumentation = [this](uint64_t request_id, nsvr_node_id id) {
		m_nodes[id].submitPlayback(request_id, SimulatedHapticNode::PlaybackCommand::Cancel);
	};

	playback->submit_pause.instrumentation = [this](uint64_t request_id, nsvr_node_id id) {
		m_nodes[id].submitPlayback(request_id, SimulatedHapticNode::PlaybackCommand::Pause);
	};

	playback->submit_cancel.instrumentation = [this](uint64_t request_id, nsvr_node_id id) {
		m_nodes[id].submitPlayback(request_id, SimulatedHapticNode::PlaybackCommand::Cancel);
	};
}

void DeviceVisualizer::simulate(double dt)
{
	for (auto& kvp : m_nodes) {
		kvp.second.update(dt);
	}
}



std::vector<RenderedNode> DeviceVisualizer::render()
{
	

	std::vector<RenderedNode> nodes;
	for (auto& kvp : m_nodes) {
		nodes.push_back(RenderedNode{ NodeId<local>(kvp.first), nsvr_node_type_haptic, kvp.second.render() });
	}
	return nodes;
}


