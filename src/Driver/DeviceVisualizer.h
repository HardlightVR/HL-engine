#pragma once
#include <unordered_map>
#include "PluginAPI.h"
#include "PluginApis.h"
#include "RenderedRegion.h"
#include "SimulatedHapticNode.h"

class DeviceVisualizer {
public:
	void provideApi(waveform_api* api);
	void provideApi(playback_api* api);
	void provideApi(buffered_api* buffered);
	void simulate(double dt);
	std::vector<RenderedNode> render();
private:
	std::unordered_map<nsvr_node_id, SimulatedHapticNode> m_nodes;
};