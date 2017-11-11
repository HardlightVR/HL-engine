#include "stdafx.h"
#include "DeviceVisualizer.h"
#include "nsvr_preset.h"
#include "DeviceIds.h"

void DeviceVisualizer::provideApi(waveform_api* waveform)
{
	waveform->submit_activate.spy = [this](uint64_t request_id, nsvr_node_id id, nsvr_default_waveform wave, uint32_t reps, float strength) {
		auto w = Waveform(request_id, wave, strength, reps);
		m_nodes[id].submitHaptic(w);
	};
}


void DeviceVisualizer::provideApi(buffered_api* buffered) {
	buffered->submit_buffer.spy = [this, buffered](uint64_t request_id, nsvr_node_id id, const double* samples, uint32_t length) {

		double dur = 0;
		buffered->submit_getsampleduration(id, &dur);
		auto w = Waveform(request_id, samples, dur, length);
		m_nodes[id].submitHaptic(w);
	};
}
void DeviceVisualizer::provideApi(playback_api* playback)
{
	playback->submit_cancel.spy = [this](uint64_t request_id, nsvr_node_id id) {
		m_nodes[id].submitPlayback(request_id, SimulatedHapticNode::PlaybackCommand::Cancel);
	}; 


	playback->submit_pause.spy = [this](uint64_t request_id, nsvr_node_id id) {
		m_nodes[id].submitPlayback(request_id, SimulatedHapticNode::PlaybackCommand::Pause);
	};

	playback->submit_cancel.spy = [this](uint64_t request_id, nsvr_node_id id) {
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
		nodes.push_back(RenderedNode{ NodeId<local>(kvp.first), nsvr_node_concept_haptic, kvp.second.render() });
	}
	return nodes;
}


