#include "stdafx.h"
#include "SimulatedDevice.h"
#include "nsvr_preset.h"
SimulatedDevice::SimulatedDevice(nsvr_device_id id, std::string name, std::string pluginName, PluginApis & apis, std::shared_ptr<BodyGraphCreator> bodygraph)
	: m_nodes()
	, m_body(bodygraph)
	, m_id(id)
	, m_name(name)
	, m_pluginName(pluginName)
{
	instrumentApis(apis);
}

void SimulatedDevice::simulate(double dt)
{
	for (auto& kvp : m_nodes) {
		kvp.second.update(dt);
	}
}

DeviceId<local> SimulatedDevice::id() const
{
	return DeviceId<local>{m_id};
}

std::vector<NodeView> SimulatedDevice::render()
{

	std::vector<NodeView> entireDevice;

	m_body->ForEachRegionPresent([this, &entireDevice](nsvr_region region, const std::vector<nsvr_node_id>& nodes) {

		NodeView view;
		view.region = region;
		for (nsvr_node_id id : nodes) {
			NodeView::SingleNode singleNode;
			singleNode.Id = id;
			//this assumption (that it is haptic) is not going to be correct going into the future
			singleNode.Type = NodeView::NodeType::Haptic;
			singleNode.DisplayData.intensity = static_cast<float>(m_nodes[id].sample());
			view.nodes.push_back(singleNode);
		}

		entireDevice.push_back(view);
	});
	


	return entireDevice;
}

std::string SimulatedDevice::name() const
{
	return m_name;
}

std::string SimulatedDevice::originatingPlugin() const
{
	return m_pluginName;
}

void SimulatedDevice::instrumentApis(PluginApis& apis)
{
	if (auto playback = apis.GetApi<playback_api>()) {
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


	if (auto waveform = apis.GetApi<waveform_api>()) {
		waveform->submit_activate.instrumentation = [this](uint64_t request_id, nsvr_node_id id, nsvr_waveform* waveform) {
			
			//need to refactor Waveform to make sense with repetitions vs sample length vs total duration. Right now it doesn't. 
			Waveform w = Waveform(request_id, waveform->waveform_id, waveform->strength, waveform->repetitions);
			m_nodes[id].submitHaptic(w);
		};
	}

	if (auto buffered = apis.GetApi<buffered_api>()) {
		
		buffered->submit_buffer.instrumentation = [this](uint64_t request_id, nsvr_node_id id, double* samples, uint32_t length) {
			Waveform w = Waveform(request_id, samples, 0.1, length);
			m_nodes[id].submitHaptic(w);

		};
	}
}
